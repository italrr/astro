#ifndef ASTRO_JOB_HPP
    #define ASTRO_JOB_HPP

    #include "Type.hpp"
    #include "Result.hpp"
    #include <functional>

    namespace astro {

        /*
            Job

            This is the fundamental code flow of astro. Most components are executed through 
            jobs.

            Types of jobs:

            - Asynchronous
                These jobs will be queued and executed in the main thread through astro::Core::update(). These are **not** threaded, and each execution will
                lock the main thread until they're done.
            - Untethered
                Completely threaded jobs. This will be executed as soon as delayStart reachs its mark.
        */

        struct Job;
        typedef std::function<void(astro::Job &ctx)> JobType;

        // It is very discouraged to change any of these values directly
        struct Job {
            int hookedJob;
            std::function<void(astro::Job &ctx)> runningjob;
            int id;
            bool infinite;
            bool running;
            bool paused;
            bool dead;
            bool untethered;
            std::vector<std::string> tags;
            uint64 delayStart;
            uint64 init;
            Job();
            void stop();
            void pause();
            void resume();
            astro::Job *hook(astro::JobType hookjob);
        };

        astro::Job *spawn(astro::JobType job, bool untethered = false, uint64 delayStart = 0, bool infinite = false, bool paused = false);
    }
    

#endif
    