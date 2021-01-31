#ifndef ASTRO_RENDER_LAYER_HPP
    #define ASTRO_RENDER_LAYER_HPP

    #include "../common/Type.hpp"

    namespace astro {
        namespace Gfx {

            enum RenderLayerType : int {
                T_2D,
                T_3D
            };

            struct RenderLayer {
                int type;
                int order;
                std::string tag;
            };
        }
    }

#endif