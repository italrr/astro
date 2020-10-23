#ifndef ASTRO_JOB_HPP
    #define ASTRO_JOB_HPP

    #include <functional>
    #include <memory>

    #include "Type.hpp"
    #include "Result.hpp"
    
    namespace astro {

        struct JobSpec {
            bool infinite;
            bool lowLatency;
            bool threaded;
            std::vector<std::string> tags;
            JobSpec(){
                infinite = false;
                lowLatency = false;
                threaded = true;
            }
        };

        enum JobStatus : uint8 {
            Running,
            Paused,
            Done
        };

        struct Job {
            int id;
            uint8 status;
            uint64 initTime;
            astro::JobSpec spec;
            std::function<void(astro::Job &ctx)> job;
            
            void stop();
            void pause();
            void resume();
            void hook();

            std::shared_ptr<astro::Job> hook(std::function<void(astro::Job &ctx)> job, const astro::JobSpec &spec);
        };

        std::shared_ptr<astro::Job> spawn(std::function<void(astro::Job &ctx)> job, const astro::JobSpec &spec);
    }
    

#endif
    