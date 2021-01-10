#include "engine/Core.hpp"
#include "engine/Graphics.hpp"
#include "engine/Input.hpp"

int main(int argc, const char *argv[]){
	
	astro::Core::init();
	astro::Gfx::init("astro", astro::Gfx::RenderEngineType::OpenGL);
	// uint64 start = astro::ticks();
	
	// int order = 0;
	// auto first = astro::spawn([&](astro::Job &ctx){
	// 	astro::log("id: %i\n", ++order);
	// }, false, false, false);

	// for(int i = 0; i < 128; ++i){
	// 	first = first->hook([&](astro::Job &ctx){
	// 		astro::log("id: %i\n", ++order);
	// 	}, false, false, false);
	// }

	while(astro::Gfx::isRunning()){
		astro::Gfx::update();
		astro::Core::update();



	}

	astro::Gfx::onEnd();
	astro::Core::onEnd();
	
	return 0;
}
