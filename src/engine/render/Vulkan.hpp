#ifndef ASTRO_RENDER_ENGINE_VULKAN_HPP
    #define ASTRO_RENDER_ENGINE_VULKAN_HPP

    #include "../Graphics.hpp"

    namespace astro {
        namespace Gfx {

            struct RenderEngineVulkan : astro::Gfx::RenderEngine {
                
                RenderEngineVulkan(){
                    this->type = RenderEngineType::Vulkan;
                }

				astro::Result init();
				astro::Result end();
				astro::Result createWindow(const std::string &title, Vec2<int> size);
				astro::Result isSupported();
                int render();

            };

        }
    }

#endif