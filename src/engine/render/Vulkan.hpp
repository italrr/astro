#ifndef ASTRO_RENDER_ENGINE_VULKAN_HPP
    #define ASTRO_RENDER_ENGINE_VULKAN_HPP

    #include "../Graphics.hpp"

    namespace astro {
        namespace Gfx {

            struct RenderEngineVulkan : astro::Gfx::RenderEngine {
                
                RenderEngineVulkan(){
                    this->type = RenderEngineType::Vulkan;
                }

				std::shared_ptr<astro::Result> init();
				std::shared_ptr<astro::Result> end();
				std::shared_ptr<astro::Result> createWindow(const std::string &title, Vec2<int> size);
				std::shared_ptr<astro::Result> isSupported();
                int render();

            };

        }
    }

#endif