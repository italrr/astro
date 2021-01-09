#ifndef ASTRO_JOB_HPP
    #define ASTRO_JOB_HPP

    #include <functional>
    #include <memory>

    #include "Type.hpp"
    #include "Result.hpp"

    namespace astro {

        struct JobSpec {
            bool looped;
            bool lowLatency;
            bool threaded;
            std::vector<std::string> tags;
            JobSpec(){
                looped = false;
                lowLatency = false;
                threaded = true;
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
            std::shared_ptr<astro::Job> hook(std::function<void(astro::Job &ctx)> funct, bool threaded);
            std::shared_ptr<astro::Job> hook(std::function<void(astro::Job &ctx)> funct, bool threaded, bool looped, bool lowLatency);
            std::shared_ptr<astro::Job> hook(std::function<void(astro::Job &ctx)> funct, const astro::JobSpec &spec);
        };

        std::shared_ptr<astro::Job> spawn(std::function<void(astro::Job &ctx)> funct, bool threaded);
        std::shared_ptr<astro::Job> spawn(std::function<void(astro::Job &ctx)> funct, bool threaded, bool looped, bool lowLatency);
        std::shared_ptr<astro::Job> spawn(std::function<void(astro::Job &ctx)> funct, const astro::JobSpec &spec);
    }
    

#endif
    