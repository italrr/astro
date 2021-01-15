#ifndef ASTRO_RENDER_TEXTURE_HPP
    #define ASTRO_RENDER_TEXTURE_HPP

    #include "../common/Type.hpp"
    #include "../common/Resource.hpp"

    namespace astro {
        namespace Gfx {

            struct Texture : astro::Resource::Resource {
                int textureId;
                Texture(){
                    type = astro::Resource::ResourceType::TEXTURE;
                    textureId = 0;
                }
                std::shared_ptr<astro::Result> unload();
                std::shared_ptr<astro::Result> load(const std::shared_ptr<astro::Indexing::Index> &file);
            };

        }
    }

#endif