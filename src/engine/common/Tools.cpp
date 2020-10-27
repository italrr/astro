#include <chrono>
#include <thread>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <random>

#include "Tools.hpp"

/*
	TIME
*/

uint64 astro::ticks(){
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

uint64 astro::epoch(){
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void astro::sleep(uint64 t){
    std::this_thread::sleep_for(std::chrono::milliseconds(t));
}

/* 
	FILE
*/
namespace astro {
	namespace File {

		bool exists(const std::string &path){
			struct stat tt;
			stat(path.c_str(), &tt);
			return S_ISREG(tt.st_mode);		
		}

	}
}

/*
	STRING
*/
std::string astro::String::toLower(const std::string &str){
	std::string out;
	for(int i = 0; i < str.size(); i++){
		out += tolower(str.at(i));
	}
	return out;
}

/* 
	MATH
*/
namespace astro {
	namespace Math {
		int random(int min, int max){
			static std::random_device rd;
			std::mt19937 rng(rd());
			std::uniform_int_distribution<int> uni(min,max);
			return uni(rng);			
		}
	}
}