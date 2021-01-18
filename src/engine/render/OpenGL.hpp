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
                std::shared_ptr<astro::Result> compileShader(const std::string &vert, const std::string &frag);
                std::shared_ptr<astro::Result> deleteShader(int id);
                int render();
                void readjustViewport(const astro::Vec2<int> &origin, const astro::Vec2<int> &size);
                // generators
                std::shared_ptr<astro::Result> generatePrimVertexBuffer(
                                                        const std::vector<float> &verts,
                                                        unsigned int nverts,
														unsigned int strides,
                                                        bool textured);             
                std::shared_ptr<astro::Result> generateTexture2D(unsigned char *data, int w, int h, int format);
                std::shared_ptr<astro::Result> generateLightSource();

                // renders
                bool renderPrimVertBuffer(astro::Gfx::RenderObject *obj);

            };

        }
    }

#endif