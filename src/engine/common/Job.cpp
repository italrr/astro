#include <pthread.h>
#include <vector>
#include <map>

#include "Tools.hpp"
#include "Log.hpp"
#include "Job.hpp"
#include "../Core.hpp"

namespace CommonMutex {
    enum CommonMutex : uint8 {
        AddJob = 0,
        Request,
        Janitor,
        Hook
    };
    static const int total = 4;
}

namespace RequestType {
    enum RequestType : uint8 {
        PAUSE = 1,
        RESUME,
        STOP,
        HOOK
    };
}

struct RequestMsg {
    uint8 type;
    int id;
    int secondary;
    astro::Job* job;
};

struct TetheredJobs {
    int jobid;
    pthread_t thread;
    astro::Job* job;
};  

struct _Jobs {
    int lastId;
    pthread_t thread;
    bool running;
    std::map<int, TetheredJobs> jobs;
    std::vector<RequestMsg> queue;
    pthread_mutex_t mutex[CommonMutex::total];
    astro::Job *spawn(astro::JobType &job, bool untethered, uint64 delayStart, bool infinite, bool paused);
    void onEnd();
    void onJobEnd(int id);
    void janitor();
    void requestHook(int to, int from);
    void request(int id, int type);   
};

static _Jobs jobs = _Jobs();

void *MainThread(void *arg){
    auto &jobs = *(_Jobs*)arg;
    uint64 lastTick = astro::ticks();
    static const uint64 tickrate = 1000 / 60;
    astro::log("Started job system | Running every %lld millisecond(s)\n", tickrate);
    while(true){
        uint64 startTick = astro::ticks();
        jobs.janitor();
        if(astro::ticks() < tickrate){
            astro::sleep(tickrate / 2);
        }
    }
}

void *Thread(void *arg){
    auto &job = *(astro::Job*)arg;
    do {
        if(job.paused || job.delayStart > 0 && astro::ticks()-job.init < job.delayStart){
            astro::sleep(20);
        }else{
            job.runningjob(job);
        }
    } while(job.running && job.infinite);
    job.running = false;
}

astro::Job *_Jobs::spawn(astro::JobType &job, bool untethered, uint64 delayStart, bool infinite, bool paused){
    pthread_mutex_lock(&mutex[CommonMutex::AddJob]);
    TetheredJobs hold;
    astro::Job *runningjob = new astro::Job();
    int cid = ++this->lastId;
    runningjob->init = astro::ticks();
    runningjob->id = cid;
    runningjob->running = true;
    runningjob->paused = paused;
    runningjob->untethered = untethered;
    runningjob->delayStart = delayStart;
    runningjob->infinite = infinite;
    runningjob->runningjob = job;
    hold.job = runningjob;
    hold.jobid = cid;
    jobs[cid] = hold;
    // spawn thread
    if(untethered){
        int r = pthread_create(&hold.thread, NULL, &Thread, (void*)runningjob);
        if(r != 0){
            astro::log("Fatal Error: Couldn't initialize untethered job's thread\n");
        }        
    }
    pthread_mutex_unlock(&mutex[CommonMutex::AddJob]);
    return runningjob;
}

void _Jobs::janitor(){
    pthread_mutex_lock(&mutex[CommonMutex::Janitor]);
    // Process messages
    for(int i = 0; i < queue.size(); i++){
        auto &msg = queue[i];
        if(msg.job->dead){
            continue;
        }
        switch(msg.type){
            case RequestType::RESUME: { 
                msg.job->paused = false;
            } break;
            case RequestType::PAUSE: { 
                msg.job->paused = true;
            } break;
            case RequestType::STOP: { 
                msg.job->running = false;
            } break;
            case RequestType::HOOK: { 
                msg.job->hookedJob = msg.secondary;
            } break;                                    
        }
    }
    queue.clear();
    pthread_mutex_unlock(&mutex[CommonMutex::Janitor]);
}

void _Jobs::request(int id, int type){
    int common = CommonMutex::Request;
    pthread_mutex_lock(&mutex[common]);
    auto job = jobs.find(id);
    if(job != jobs.end() && !job->second.job->dead){
        RequestMsg msg;
        msg.id = job->second.jobid;
        msg.type = type;
        msg.job = job->second.job;
        queue.push_back(msg);
    }
    pthread_mutex_unlock(&mutex[common]);
}

void _Jobs::requestHook(int to, int from){
    int common = CommonMutex::Request;
    pthread_mutex_lock(&mutex[common]);
    auto job = jobs.find(to);
    if(job != jobs.end() && !job->second.job->dead){
        RequestMsg msg;
        msg.id = job->second.jobid;
        msg.secondary = from;
        msg.type = RequestType::HOOK;
        msg.job = job->second.job;
        queue.push_back(msg);
    }    
    pthread_mutex_unlock(&mutex[common]);
}

void _Jobs::onJobEnd(int id){
    auto current = jobs.find(id);
    if(current == jobs.end()){
        return;
    }
    auto job = current->second.job;
    job->dead = true;
    if(job->hookedJob != 0){
        auto hooked = jobs.find(job->hookedJob);
        if(hooked != jobs.end() && hooked->second.job->paused){
            request(job->hookedJob, RequestType::RESUME);
        }
    }
}

void _Jobs::onEnd(){
    // TODO: join all jobs
}

void __ASTRO_init_job(){
    jobs.lastId = 1;
    int r = pthread_create(&jobs.thread, NULL, &MainThread, (void*)&jobs);
    if(r != 0){
        astro::log("Fatal Error: Couldn't initialize astro::Job's main thread. Exiting...\n");
        astro::Core::exit(1);
    }
    for(int i = 0; i < CommonMutex::total; ++i){
        if(pthread_mutex_init(&jobs.mutex[i], NULL) != 0){ 
            astro::log("Fatal Error: Couldn't initialize astro::Job's main mutex. Exiting...\n");
            astro::Core::exit(1);
        } 
    }       
    jobs.running = true;    
}

void __ASTRO_end_job(){
    jobs.onEnd();
    for(int i = 0; i < CommonMutex::total; ++i){
        pthread_mutex_destroy(&jobs.mutex[i]);
    }      
}

void __ASTRO_update_job(){
    for(auto &job : jobs.jobs){
        auto *ref = job.second.job;
        if(!ref->untethered && ref->running && !ref->dead && !ref->paused){
            if(astro::ticks()-ref->init > ref->delayStart){
                ref->runningjob(*ref);
                if(!ref->infinite){
                    jobs.request(ref->id, RequestType::STOP);
                }
            }
        }
        if(!ref->running && !ref->dead){
            jobs.onJobEnd(job.second.jobid);
        }
    }
}

astro::Job::Job(){
    hookedJob = 0;
    runningjob = [](astro::Job &ctx){ return; };
}

void astro::Job::stop(){
    jobs.request(this->id, RequestType::STOP);
}

void astro::Job::pause(){
    jobs.request(this->id, RequestType::PAUSE);
}

void astro::Job::resume(){
    jobs.request(this->id, RequestType::RESUME);
}

astro::Job *astro::Job::hook(astro::JobType hookjob){
    auto job = jobs.spawn(hookjob, this->untethered, 0, false, true);
    jobs.requestHook(this->id, job->id);
    return job;
}

astro::Job *astro::spawn(astro::JobType job, bool untethered, uint64 delayStart, bool infinite, bool paused){
    return jobs.spawn(job, untethered, delayStart, infinite, paused);
}