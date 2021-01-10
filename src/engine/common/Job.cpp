#include <pthread.h>
#include <vector>
#include <map>

#include "Log.hpp"
#include "Job.hpp"
#include "Tools.hpp"


/*

    Scheduler

*/

struct _Job {
    int id;
    int hooked;
    int lead;
    _Job(){
        id = 0;
        hooked = 0;
    }
    pthread_t thread;
    std::shared_ptr<astro::Job> handle;
    std::function<void(astro::Job &ctx)> funct;
};
static std::map<int, _Job> pool =  {};

static void *thread_job(void *ctx){
    _Job *job = static_cast<_Job*>(ctx);
    while(job->handle->status == astro::JobStatus::Waiting){
        astro::sleep(16);
    }
    if(job->handle->status != astro::JobStatus::Running){
        pthread_exit(0);
        return NULL;
    }
    auto &spec = job->handle->spec;
    job->handle->initTime = astro::ticks();
    do {
        job->funct(*job->handle.get());
        if(spec.looped && !spec.lowLatency){
            astro::sleep(16);
        }
    } while (spec.looped && job->handle->status == astro::JobStatus::Running);
    job->handle->status = astro::JobStatus::Done;
    // TODO: call hook
    pthread_exit(0);
    return NULL;
}

struct _Scheduler {
    int startId;
    pthread_mutex_t genIdMutex;
    pthread_mutex_t poolMutex;
    uint64 lastTick;

    void init(){
        startId = astro::Math::random(25, 50);
        pthread_mutex_init(&genIdMutex, NULL);
        pthread_mutex_init(&poolMutex, NULL);
    }

    void end(){
        // stop everything and clean up
        pthread_mutex_lock(&poolMutex);
        astro::log("[JOB] stopping all jobs(%i)...\n", pool.size());
        for(auto &it : pool){
            it.second.handle->status = astro::JobStatus::Stopped;
        }
        for(auto &it : pool){
            if(it.second.handle->spec.threaded){
                pthread_join(it.second.thread, NULL); 
            }
        }        
        while(pool.size() > 0){ janitor(false); }
        pthread_mutex_unlock(&poolMutex);
        pthread_mutex_destroy(&genIdMutex); 
        pthread_mutex_destroy(&poolMutex);
        astro::log("[JOB] done\n");
    }

    std::shared_ptr<astro::Job> spawn(std::function<void(astro::Job &ctx)> funct, const astro::JobSpec &spec){
        int id = generateId();
        std::shared_ptr<astro::Job> handle = std::shared_ptr<astro::Job>(new astro::Job());
        handle->id = id;
        handle->status = astro::JobStatus::Waiting;
        handle->spec = spec;
        _Job job = _Job();
        job.id = id;
        job.handle = handle;
        job.hooked = 0;
        job.lead = 0;
        job.funct = funct;
        pthread_mutex_lock(&poolMutex);
        pool[id] = job;
        if(spec.threaded){
            int rc  = pthread_create(&job.thread, NULL, thread_job, (void*)&pool[id]);
            if (rc){
                astro::log("[JOB] attention: failed to spawn thread id '%i'. return code: '%i'\n", id, rc);
                handle->status = astro::JobStatus::Stopped;
            }            
        }
        pthread_mutex_unlock(&poolMutex);
        return handle;
    }


    std::vector<std::shared_ptr<astro::Job>> findJobs(const std::vector<std::string> &tags, int minmatch = 1){
        std::vector<std::shared_ptr<astro::Job>> jobs;
        pthread_mutex_lock(&poolMutex);
        int matches = 0;
        auto findMatches = [](const std::vector<std::string> &from, const std::vector<std::string> &tags){
            int n = 0;
            if(from.size() == 0){
                return n;
            }
            for(int i = 0; i < from.size(); ++i){
                for(int j = 0; j < tags.size(); ++j){
                    if(astro::String::toLower(from[i]) == astro::String::toLower(tags[j])){
                        ++n;
                    }
                }
            }
            return n;
        };
        for(auto &it : pool){
            auto &j = it.second.handle;
            int nmatches = findMatches(j->spec.tags, tags);
            if(nmatches >= minmatch){
                jobs.push_back(j);
            }
        }
        pthread_mutex_unlock(&poolMutex);        
        return jobs;
    }

    std::shared_ptr<astro::Job> findJob(const std::vector<std::string> &tags, int minmatch = 1){
        auto job = std::shared_ptr<astro::Job>(NULL);
        pthread_mutex_lock(&poolMutex);
        int matches = 0;
        auto findMatches = [](const std::vector<std::string> &from, const std::vector<std::string> &tags){
            int n = 0;
            if(from.size() == 0){
                return n;
            }
            for(int i = 0; i < from.size(); ++i){
                for(int j = 0; j < tags.size(); ++j){
                    if(astro::String::toLower(from[i]) == astro::String::toLower(tags[j])){
                        ++n;
                    }
                }
            }
            return n;
        };
        for(auto &it : pool){
            auto &j = it.second.handle;
            int nmatches = findMatches(j->spec.tags, tags);
            if(nmatches >= minmatch){
                job = j;
                break;
            }
        }
        pthread_mutex_unlock(&poolMutex);        
        return job;
    }

    std::shared_ptr<astro::Job> findJob(int id){
        auto job = std::shared_ptr<astro::Job>(NULL);
        pthread_mutex_lock(&poolMutex);
        auto it = pool.find(id);
        if(it != pool.end()){
            job = it->second.handle;
        }
        pthread_mutex_unlock(&poolMutex);        
        return job;
    }

    std::shared_ptr<astro::Job> hook(int lead, std::function<void(astro::Job &ctx)> funct, const astro::JobSpec &spec){
        int id = generateId();
        std::shared_ptr<astro::Job> handle = std::shared_ptr<astro::Job>(new astro::Job());
        handle->id = id;
        handle->status = astro::JobStatus::Waiting;
        handle->spec = spec;
        _Job job = _Job();
        job.id = id;
        job.handle = handle;
        job.hooked = 0;
        job.lead = 0;
        job.funct = funct;
        pthread_mutex_lock(&poolMutex);
        pool[id] = job;
        auto &jref = pool[id];
        auto it = pool.find(lead);
        if(it != pool.end()){
            it->second.hooked = id;
            jref.lead = lead;
            handle->status = astro::JobStatus::Waiting;
            handle->payload = it->second.handle->payload; // inherit payload from lead
        }else{
            astro::log("[JOB] failed to hook job to non-existing job id %i. starting it right away instead...\n", lead);
        }
        if(spec.threaded){
            int rc  = pthread_create(&jref.thread, NULL, thread_job, (void*)&pool[id]);
            if (rc){
                astro::log("[JOB] attention: failed to spawn thread id '%i'. return code: '%i'\n", id, rc);
                handle->status = astro::JobStatus::Stopped;
            }            
        }

        pthread_mutex_unlock(&poolMutex);
        return handle;
    }

    void stop(astro::Job *job){
        job->status = astro::JobStatus::Stopped;     
    }

    void janitor(bool lock = true){
        if(lock) pthread_mutex_lock(&poolMutex);
        std::vector<int> toRemove;
        for(auto &it : pool){
            auto &job = it.second;
            auto *handle = it.second.handle.get();
            switch(handle->status){
                case astro::JobStatus::Done:
                case astro::JobStatus::Stopped: {
                    if(job.hooked != 0){
                        auto hit = pool.find(job.hooked);
                        if(hit != pool.end()){
                            hit->second.handle->status = astro::JobStatus::Running;
                            job.hooked = 0;
                        }
                    }
                    toRemove.push_back(handle->id);
                    handle->id = 0;
                } break;
                case astro::JobStatus::Waiting: {
                    auto it = pool.find(job.lead);
                    if(it == pool.end()){
                        job.handle->status = astro::JobStatus::Running;
                    }
                } break;
                case astro::JobStatus::Running: {
                    if(handle->spec.threaded){
                        break;
                    }
                    job.funct(*handle);
                    if(!handle->spec.looped){
                        handle->status = astro::JobStatus::Done;
                        toRemove.push_back(handle->id);
                        handle->id = 0;
                    }
                } break;
            }
        }
        for(int i = 0; i < toRemove.size(); ++i){
            pool.erase(toRemove[i]);
        }
        if(lock)  pthread_mutex_unlock(&poolMutex);       
    }

    int generateId(){
        pthread_mutex_lock(&genIdMutex);
        int nid = ++startId;
        pthread_mutex_unlock(&genIdMutex);
        return nid;
    }

};
static _Scheduler sch = _Scheduler();

/*

    Job

*/

void astro::Job::stop(){
    sch.stop(this);
}

astro::Job::Job(){
    this->payload = std::make_shared<astro::SmallPacket>(astro::SmallPacket());
}

std::shared_ptr<astro::Job> astro::Job::hook(std::function<void(astro::Job &ctx)> funct, bool threaded){
    astro::JobSpec spec;
    spec.looped = false;
    spec.threaded = threaded;
    spec.lowLatency = false;
    return sch.hook(id, funct, spec);
}

std::shared_ptr<astro::Job> astro::Job::hook(std::function<void(astro::Job &ctx)> funct, const astro::JobSpec &spec){
    return sch.hook(id, funct, spec);
}

std::shared_ptr<astro::Job> astro::Job::hook(std::function<void(astro::Job &ctx)> funct, bool threaded, bool looped, bool lowLatency){
    astro::JobSpec spec;
    spec.looped = looped;
    spec.threaded = threaded;
    spec.lowLatency = lowLatency;
    return sch.hook(id, funct, spec);
}

std::shared_ptr<astro::Job> astro::spawn(std::function<void(astro::Job &ctx)> funct, bool threaded){
    astro::JobSpec spec;
    spec.looped = false;
    spec.threaded = threaded;
    spec.lowLatency = false;
    return spawn(funct, spec);
}

std::shared_ptr<astro::Job> astro::spawn(std::function<void(astro::Job &ctx)> funct, bool threaded, bool looped, bool lowLatency){
    astro::JobSpec spec;
    spec.looped = looped;
    spec.threaded = threaded;
    spec.lowLatency = lowLatency;
    return spawn(funct, spec);
}

std::shared_ptr<astro::Job> astro::spawn(std::function<void(astro::Job &ctx)> funct, const astro::JobSpec &spec){
    return sch.spawn(funct, spec);
}

std::vector<std::shared_ptr<astro::Job>> astro::findJobs(const std::vector<std::string> &tags, int minmatch){
    return sch.findJobs(tags, minmatch);
}

std::shared_ptr<astro::Job> astro::findJob(const std::vector<std::string> &tags, int minmatch){
    return sch.findJob(tags, minmatch);
}

std::shared_ptr<astro::Job> astro::findJob(int id){
    return sch.findJob(id);
}

/*

    astro module interface

*/
void __ASTRO_init_job(){
    sch.init();
}

void __ASTRO_end_job(){
    sch.end();
    // this is hanging on windows...
    // TODO: check it out later
    // pthread_exit(NULL);
}

void __ASTRO_update_job(){
    sch.janitor();
}
