#ifndef ASTRO_RENDER_MODEL_HPP
    #define ASTRO_RENDER_MODEL_HPP

    #include "Texture.hpp"
    #include "../Graphics.hpp"

    namespace astro {
        namespace Gfx {

            struct Mesh : astro::Gfx::RenderObject {
                unsigned int vao;
                unsigned int vbo;
                unsigned int ebo;                 
                std::vector<astro::Gfx::Vertex> vertices;
                std::vector<unsigned int> indices;          
                Mesh(){
                    rObjtype = RenderObjectType::MESH;
                    vao = 0;
                    vbo = 0;
                    ebo = 0;
                }
                void render();
            };

            struct Model : astro::Resource::Resource, astro::Gfx::RenderObject {
                std::vector<std::shared_ptr<astro::Gfx::Mesh>> meshes;       
                std::shared_ptr<astro::Result> unload();
                std::shared_ptr<astro::Result> load(const std::shared_ptr<astro::Indexing::Index> &file);   
                void render();
                Model(){
                    rObjtype = astro::Gfx::RenderObjectType::MODEL;
                    rscType = astro::Resource::ResourceType::MODEL;
                }                       
            };        

        }
    }

#endif