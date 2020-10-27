#include "engine/Graphics.hpp"
#include "engine/Core.hpp"

int main(int argc, const char *argv[]){
	
	astro::Core::init();
	astro::Gfx::init("astro", astro::Gfx::RenderEngineType::OpenGL);

	while(astro::Gfx::isRunning()){

		astro::Gfx::update();
		astro::Core::update();

	}

	astro::Gfx::onEnd();
	astro::Core::onEnd();
	
	return 0;
}
