#ifndef ASTRO_RENDER_ENGINE_OPENGL_HPP
    #define ASTRO_RENDER_ENGINE_OPENGL_HPP

    #include "../Graphics.hpp"

    namespace astro {
        namespace Gfx {

            struct RenderEngineOpenGL : astro::Gfx::RenderEngine {
                
                RenderEngineOpenGL(){
                    this->type = RenderEngineType::OpenGL;
                }

				std::shared_ptr<astro::Result> init();
				std::shared_ptr<astro::Result> end();
				std::shared_ptr<astro::Result> createWindow(const std::string &title, Vec2<int> size);
				std::shared_ptr<astro::Result> isSupported();
                int render();
                void readjustViewport(const astro::Vec2<int> &origin, const astro::Vec2<int> &size);

            };

        }
    }

#endif