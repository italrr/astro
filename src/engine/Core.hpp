#ifndef ASTRO_CORE_HPP
    #define ASTRO_CORE_HPP

    #include <string>
    
    #include "common/Log.hpp"
    #include "common/Job.hpp"

    namespace astro {
        namespace Core {
            void exit(int code = 0);
            void init();
            void update();
            void onEnd();
        }
    }
    

#endif
    