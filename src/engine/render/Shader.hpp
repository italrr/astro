#ifndef ASTRO_RENDER_SHADER_HPP
    #define ASTRO_RENDER_SHADER_HPP

    #include "../common/Resource.hpp"

    namespace astro {
        namespace Gfx {

            struct Shader : astro::Resource::Resource {
                std::string vertSrc;
                std::string fragSrc;
                int shaderId;
                Shader(){
                    type = astro::Resource::ResourceType::SHADER;
                    shaderId = 0;
                }
                std::shared_ptr<astro::Result> unload();
                std::shared_ptr<astro::Result> load(const std::shared_ptr<astro::Indexing::Index> &file);
            };

        }
    }

#endif