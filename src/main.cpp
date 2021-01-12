#include "engine/Core.hpp"
#include "engine/Graphics.hpp"
#include "engine/Input.hpp"

int main(int argc, const char *argv[]){
	
	astro::Core::init();
	bool started = false;
	
	//
	// Rendering thread
	//
	auto gfxthrd = astro::spawn([&](astro::Job &ctx){ // loop
		astro::Gfx::update();
		if(!astro::Gfx::isRunning()){
			ctx.stop();
		}		
	}, astro::JobSpec(true, true, true, {"astro_gfx"}));
	gfxthrd->setOnStart([&](astro::Job &ctx){ // init
		astro::Gfx::init("astro", astro::Gfx::RenderEngineType::OpenGL);
		started = true;		
	});
	gfxthrd->setOnEnd([&](astro::Job &ctx){ // end
		astro::Gfx::onEnd();
	});
	// wait for render thread to start
	while(!started){
		astro::Core::update();
		astro::sleep(100); 
	}

	//
	// Main thread
	//
	do {
		astro::Core::update();
	} while(astro::Gfx::isRunning());

	astro::Core::onEnd();
	
	return 0;
}
