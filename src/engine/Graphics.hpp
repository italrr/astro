#ifndef ASTRO_GRAPHICS_HPP
	#define ASTRO_GRAPHICS_HPP

	#include "common/Type.hpp"
	#include "common/Result.hpp"

	namespace astro {
		namespace Gfx {

			namespace RenderEngineType {
				enum RenderEngineType : int {
					OpenGL = 0,
					Vulkan,
					DirectX,
					Software,
					None
				};
				static std::string name(int type){
					switch(type){
						case RenderEngineType::OpenGL:
							return "OpenGL";
						case RenderEngineType::Vulkan:
							return "Vulkan";
						case RenderEngineType::DirectX:
							return "DirectX";
						case RenderEngineType::Software:
							return "Software";													
						case RenderEngineType::None:
							return "None";								
						default:
							return "Undefined";
					}
				}
			}

			struct RenderEngine {
				RenderEngine(){
					type = RenderEngineType::None;
				}
				int type;
				virtual astro::Result init(){ return Result(ResultType::Success); }
				virtual astro::Result end(){ return Result(ResultType::Success); }
				virtual astro::Result createWindow(const std::string &title, Vec2<int> size){ return Result(ResultType::Success); }
				virtual astro::Result isSupported(){ return Result(ResultType::Success); }
				virtual astro::Result render(){ return Result(ResultType::Success); } 
			};
			
			void init(const std::string &title, astro::Vec2<int> size = astro::Vec2<int>(0));
			void update();
			bool isRunning();
			void onEnd();
			void end();

		}
	}

#endif