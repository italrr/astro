#include <cstdlib>

#include "common/Log.hpp"
#include "Core.hpp"

// astro's internal controllers
void __ASTRO_init_log();
void __ASTRO_end_log();
void __ASTRO_init_job();
void __ASTRO_update_job();
void __ASTRO_end_job();


/*
    Initializes internal modules for astro
*/
void astro::Core::init(){
    __ASTRO_init_log();
    __ASTRO_init_job();
}

/*
    Assures a graceful termination of astro's internal modules
*/
void astro::Core::onEnd(){
    __ASTRO_end_job();
    __ASTRO_end_log();
}

/*
    Requests exit of the main process. This is usually used to
    forcefully terminate the engine.
*/
void astro::Core::exit(int code){
    std::exit(code);
}

/*
    Updates astro's internal modules
*/
void astro::Core::update(){
    __ASTRO_update_job();
}