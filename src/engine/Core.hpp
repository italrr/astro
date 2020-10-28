#ifndef ASTRO_CORE_HPP
    #define ASTRO_CORE_HPP

    #include <string>
    
    #include "common/Log.hpp"
    #include "common/Job.hpp"

    namespace astro {

        namespace Core {

            struct SettingsFile {
                // render
                astro::Vec2<int> size; // window size
                bool vsync;
                bool resizeable;
                bool fullscreen;
                std::string backend; // opengl, vulkan
                // mouse
                std::string mouseMode;
            };

            void exit(int code = 0);
            void init();
            void update();
            void onEnd();
            // this is non-async
            astro::Core::SettingsFile getSettingsFile();
        }
    }
    

#endif
    