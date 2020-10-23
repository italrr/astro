#ifndef ASTRO_TOOL_GENERAL_HPP
    #define ASTRO_TOOL_GENERAL_HPP

    #include "Type.hpp"
    #include "3rdparty/json.hpp"

    namespace astro {
        // time
        uint64 ticks();
        uint64 epoch();
        void sleep(uint64 t);
    }

#endif