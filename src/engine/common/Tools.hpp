#ifndef ASTRO_TOOL_GENERAL_HPP
    #define ASTRO_TOOL_GENERAL_HPP

    #include "Type.hpp"

    namespace astro {
        // time
        uint64 ticks();
        uint64 epoch();
        void sleep(uint64 t);
        // file
        namespace File {
            bool exists(const std::string &path);
        }
        // string
        namespace String {
            std::string toLower(const std::string &str);
        }
    }

#endif