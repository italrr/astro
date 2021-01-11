#ifndef ASTRO_JOB_HPP
    #define ASTRO_JOB_HPP

    #include <functional>
    #include <memory>

    #include "Type.hpp"
    #include "Tools.hpp"
    #include "Result.hpp"

    namespace astro {

        struct JobSpec {
            bool looped;
            bool lowLatency;
            bool threaded;
            std::shared_ptr<astro::SmallPacket> payload;
            std::vector<std::string> tags;
            std::shared_ptr<astro::Result> result;
            JobSpec(){
                this->looped = false;
                this->lowLatency = false;
                this->threaded = true;
                this->payload = std::make_shared<astro::SmallPacket>(astro::SmallPacket());
            }
            JobSpec(bool threaded, bool looped, bool lowLatency, const std::vector<std::string> &tags = {}){
                this->threaded = threaded;
                this->looped = looped;
                this->lowLatency = lowLatency;
                this->tags = tags;
                this->payload = std::make_shared<astro::SmallPacket>(astro::SmallPacket());
            }
            bool hasTag(const std::string &tag){
                for(int i = 0; i < this->tags.size(); ++i){
                    if(this->tags[i] == tag){
                        return true;
                    }
                }
                return false;
            }
        };

        enum JobStatus : uint8 {
            Stopped,
            Running,
            Waiting,
            Done
        };

        struct Job {
            // do not change these
            int id;
            uint8 status;
            uint64 initTime;
            astro::JobSpec spec;
            // interfacing
            void stop();
            Job();
            std::shared_ptr<astro::Job> hook(std::function<void(astro::Job &ctx)> funct, bool threaded);
            std::shared_ptr<astro::Job> hook(std::function<void(astro::Job &ctx)> funct, bool threaded, bool looped, bool lowLatency);
            std::shared_ptr<astro::Job> hook(std::function<void(astro::Job &ctx)> funct, const astro::JobSpec &spec);
        };

        std::vector<std::shared_ptr<astro::Job>> findJobs(const std::vector<std::string> &tags, int minmatch = 1);
        std::shared_ptr<astro::Job> findJob(const std::vector<std::string> &tags, int minmatch = 1);
        std::shared_ptr<astro::Job> findJob(int id);

        std::shared_ptr<astro::Job> spawn(std::function<void(astro::Job &ctx)> funct, bool threaded);
        std::shared_ptr<astro::Job> spawn(std::function<void(astro::Job &ctx)> funct, bool threaded, bool looped, bool lowLatency);
        std::shared_ptr<astro::Job> spawn(std::function<void(astro::Job &ctx)> funct, const astro::JobSpec &spec);
    }
    

#endif
    