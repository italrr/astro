#ifndef ASTRO_GRAPHICS_HPP
	#define ASTRO_GRAPHICS_HPP

	#include "Type.hpp"

	namespace astro {
		namespace Gfx {
			
			void init(const std::string &title, astro::Vec2<int> size);
			void update();
			bool isRunning();
			void onEnd();
			void end();

		}
	}

#endif