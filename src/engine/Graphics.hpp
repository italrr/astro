#ifndef ASTRO_GRAPHICS_HPP
	#define ASTRO_GRAPHICS_HPP

	#include "common/Type.hpp"
	#include "common/Result.hpp"
	#include "common/Tools.hpp"

	namespace astro {
		namespace Gfx {

			namespace RenderEngineType {
				enum RenderEngineType : int {
					Undefined = -1,
					OpenGL = 0,
					Vulkan,
					DirectX,
					Software
				};
				static RenderEngineType value(const std::string &name){
					if(astro::String::toLower(name) == "opengl"){
						return RenderEngineType::OpenGL;
					}else
					if(astro::String::toLower(name) == "vulkan"){
						return RenderEngineType::Vulkan;
					}else
					if(astro::String::toLower(name) == "directx"){
						return RenderEngineType::DirectX;
					}else
					if(astro::String::toLower(name) == "software"){
						return RenderEngineType::Software;
					}else{
						return RenderEngineType::Undefined;
					}																	
				}
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
						default:
							return "Undefined";
					}
				}
			}

			struct RenderEngine {
				RenderEngine(){
					type = RenderEngineType::Undefined;
				}
				int type;
				virtual astro::Result init(){ return Result(ResultType::Success); }
				virtual astro::Result end(){ return Result(ResultType::Success); }
				virtual astro::Result createWindow(const std::string &title, Vec2<int> size){ return Result(ResultType::Success); }
				virtual astro::Result isSupported(){ return Result(ResultType::Success); }
				virtual astro::Result render(){ return Result(ResultType::Success); } 
			};
			

			struct RenderInitSettings {
				RenderInitSettings(){
					size = astro::Vec2<int>(0);
					title = "";
					ret = RenderEngineType::Undefined;
				}
				std::string title;
				astro::Vec2<int> size;
				RenderEngineType::RenderEngineType ret;
			};

			void init(const std::string &title, astro::Vec2<int> size = astro::Vec2<int>(0));
			void init(const std::string &title, RenderEngineType::RenderEngineType ret = RenderEngineType::Undefined);
			void init(const std::string &title, RenderInitSettings &sett);
			void init(RenderInitSettings &sett);
			void update();
			bool isRunning();
			void onEnd();
			void end();

		}
	}

#endif