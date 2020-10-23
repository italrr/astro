#include <cstdlib>

#include "common/3rdparty/Jzon.hpp"
#include "common/Log.hpp"
#include "common/Tools.hpp"
#include "Core.hpp"

#define DEFAULT_SETTINGS_FILE "data/settings.json"
#define DEFAULT_SETTINGS_BACKEND "opengl"
#define DEFAULT_SETTINGS_WSIZE astro::Vec2<int>(640, 480)
#define DEFAULT_SETTINGS_VSYNC false

static astro::Core::SettingsFile sfile; 

astro::Core::SettingsFile astro::Core::getSettingsFile(){
    return sfile;
}


// astro's internal controllers
void __ASTRO_init_log();
void __ASTRO_end_log();
void __ASTRO_init_job();
void __ASTRO_update_job();
void __ASTRO_end_job();



void astro::Core::init(){
    // read settings file
    Jzon::Parser parser;
    auto settings = parser.parseFile(DEFAULT_SETTINGS_FILE);
    sfile.size.x = settings.get("rwidth").toInt(DEFAULT_SETTINGS_WSIZE.x);
    sfile.size.y = settings.get("rheight").toInt(DEFAULT_SETTINGS_WSIZE.y);
    sfile.vsync = settings.get("rvsync").toBool(DEFAULT_SETTINGS_VSYNC);
    sfile.backend = settings.get("rbackend").toString(DEFAULT_SETTINGS_BACKEND);   
    __ASTRO_init_log();
    __ASTRO_init_job();
}

void astro::Core::onEnd(){
    __ASTRO_end_job();
    __ASTRO_end_log();
}

void astro::Core::exit(int code){
    std::exit(code);
}

void astro::Core::update(){
    __ASTRO_update_job();
}