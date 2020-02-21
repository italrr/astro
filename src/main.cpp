#include "engine/Graphics.hpp"
#include "engine/Core.hpp"

int main(int argc, const char *argv[]){
	
	astro::Core::init();
	astro::Gfx::init("astro ~> *", astro::Vec2<int>(1920, 1080));


	auto job = astro::spawn([](astro::Job &ctx){
		astro::log("first!\n");
	}, false, 10000, false);

	job->hook([](astro::Job &ctx){
		astro::log("second!\n");
	});

	while(astro::Gfx::isRunning()){

		astro::Gfx::update();
		astro::Core::update();

	}

	astro::Gfx::onEnd();
	astro::Core::onEnd();
	
	return 0;
}
