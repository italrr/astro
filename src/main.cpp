#include "engine/Graphics.hpp"

int main(int argc, const char *argv[]){
	
	astro::Gfx::init("Cube 3D", astro::Vec2<int>(1920, 1080));

	while(astro::Gfx::isRunning()){

		astro::Gfx::update();

	}

	astro::Gfx::onEnd();
	
	return 0;
}
