#ifndef ASTRO_CORE_HPP
    #define ASTRO_CORE_HPP

    #include <string>
    
    #include "common/Log.hpp"
    #include "common/Job.hpp"
    #include "common/Indexer.hpp"
    #include "common/Resource.hpp"

    namespace astro {

        namespace Core {

            namespace SupportedPlatform {
                enum SupportedPlatform : int {
                    WINDOWS,
                    LINUX,
                    UNSUPPORTED = -1
                };
                static std::string name(int plat){
                    switch(plat){
                        case SupportedPlatform::WINDOWS: 
                            return "WINDOWS";
                        case SupportedPlatform::LINUX:
                            return "LINUX";
                        default:
                            return "UNSUPPORTED";
                    }
                }
            }

            #ifdef _WIN32
                const static int PLATFORM = SupportedPlatform::WINDOWS;
            #elif __linux__
                const static int PLATFORM = SupportedPlatform::LINUX;
            #else
                const static int PLATFORM = SupportedPlatform::UNSUPPORTED;
            #endif       

            static std::string CurrentPlatform(){
                return SupportedPlatform::name(Core::PLATFORM);
            }     

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
            astro::Indexing::Indexer *getIndexer();
            astro::Resource::ResourceManager *getResourceMngr();
            // this is non-async
            astro::Core::SettingsFile getSettingsFile();
        }
    }
    

#endif
    