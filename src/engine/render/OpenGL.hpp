#ifndef ASTRO_RENDER_ENGINE_OPENGL_HPP
    #define ASTRO_RENDER_ENGINE_OPENGL_HPP

    #include "../Graphics.hpp"

    namespace astro {
        namespace Gfx {

            struct RenderEngineOpenGL : astro::Gfx::RenderEngine {
                
                RenderEngineOpenGL(){
                    this->type = RenderEngineType::OpenGL;
                }

				astro::Result init();
				astro::Result end();
				astro::Result createWindow(const std::string &title, Vec2<int> size);
				astro::Result isSupported();
                int render();
                void readjustViewport(const astro::Vec2<int> &origin, const astro::Vec2<int> &size);

            };

        }
    }

#endif