#include <cstdlib>

#include "common/3rdparty/Jzon.hpp"
#include "common/Log.hpp"
#include "common/Tools.hpp"
#include "Core.hpp"

#define DEFAULT_SETTINGS_FILE "data/settings.json"
#define DEFAULT_SETTINGS_BACKEND "opengl"
#define DEFAULT_SETTINGS_WSIZE astro::Vec2<int>(640, 480)
#define DEFAULT_SETTINGS_VSYNC false
#define DEFAULT_SETTINGS_RESIZEABLE false
#define DEFAULT_SETTINGS_FULLSCREEN false

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
    auto rsettings = settings.get("render"); // TODO: add some validation
    sfile.size.x = rsettings.get("width").toInt(DEFAULT_SETTINGS_WSIZE.x);
    sfile.size.y = rsettings.get("height").toInt(DEFAULT_SETTINGS_WSIZE.y);
    sfile.vsync = rsettings.get("vsync").toBool(DEFAULT_SETTINGS_VSYNC);
    sfile.backend = rsettings.get("backend").toString(DEFAULT_SETTINGS_BACKEND); 
    sfile.resizeable = rsettings.get("resizeable").toBool(DEFAULT_SETTINGS_RESIZEABLE);
    sfile.fullscreen = rsettings.get("fullscreen").toBool(DEFAULT_SETTINGS_FULLSCREEN);
    __ASTRO_init_log();
    __ASTRO_init_job();
    astro::log("astro ~> *\n");
}

void astro::Core::onEnd(){
    __ASTRO_end_job();
    astro::log("bye*\n");
    __ASTRO_end_log();
}

void astro::Core::exit(int code){
    std::exit(code);
}

void astro::Core::update(){
    __ASTRO_update_job();
}