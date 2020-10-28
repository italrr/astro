#include <GLFW/glfw3.h>
#include <signal.h>

#include "common/3rdparty/Jzon.hpp"
#include "common/Log.hpp"
#include "common/Job.hpp"
#include "Graphics.hpp"
#include "Core.hpp"
#include "render/Vulkan.hpp"
#include "render/OpenGL.hpp"

// since input comes from the window itself
void __ASTRO_init_input(GLFWwindow *window);
void __ASTRO_end_input();

struct _Gfx {
    std::string title;
    float timescale;
    bool running;
    astro::Vec2<int> size;
    GLFWwindow *window;
    astro::Gfx::RenderEngine *render;
    _Gfx(){
        this->title = "astro";
        this->timescale = 1.0f;
        this->running = false;
        this->size = astro::Vec2<int>(640, 480);
        this->window = NULL;
        this->render = NULL;
    }
    void setBackend(astro::Gfx::RenderEngineType::RenderEngineType type){
        switch(type){
            case astro::Gfx::RenderEngineType::OpenGL: {
                this->render = new astro::Gfx::RenderEngineOpenGL();
            } break;
            case astro::Gfx::RenderEngineType::Vulkan: {
                this->render = new astro::Gfx::RenderEngineVulkan();
            } break;            
            default: {
                astro::log("fatal error: only rendering backends availaible are OpenGL and Vulkan\n");
                astro::Core::exit(1);
            } break;                                                
        }
    }
    ~_Gfx(){
        if(this->render != NULL){
            return;
        }
        delete this->render;
    }
};

static _Gfx ctx = _Gfx();

static void ctrlC(int s){
    astro::log("caught ctrl-c\n");
	astro::Gfx::end();
}

void astro::Gfx::init(const std::string &title, astro::Vec2<int> size){
    auto sfile = astro::Core::getSettingsFile();
    RenderInitSettings sett = RenderInitSettings();
    sett.title = title;
    sett.size = size;
    sett.vsync = sfile.vsync;
    sett.fullscreen = sfile.vsync;
    sett.resizeable = sfile.resizeable;
    sett.ret = RenderEngineType::value(sfile.backend);
    init(sett);
}

void astro::Gfx::init(const std::string &title, astro::Gfx::RenderInitSettings &sett){
    sett.title = title;
    init(sett);
}

void astro::Gfx::init(const std::string &title, RenderEngineType::RenderEngineType ret){  
    auto sfile = astro::Core::getSettingsFile();
    RenderInitSettings sett = RenderInitSettings();
    sett.title = title;
    sett.size = sfile.size;
    sett.vsync = sfile.vsync;
    sett.fullscreen = sfile.fullscreen;
    sett.resizeable = sfile.resizeable;
    sett.ret = ret;
    init(sett);  
}

void astro::Gfx::init(const std::string &title){
    auto sfile = astro::Core::getSettingsFile();
    RenderInitSettings sett = RenderInitSettings();
    sett.title = title;
    sett.size = sfile.size;
    sett.vsync = sfile.vsync;
    sett.fullscreen = sfile.fullscreen;
    sett.resizeable = sfile.resizeable;
    sett.ret = RenderEngineType::value(sfile.backend);
    init(sett);
}

void astro::Gfx::init(RenderInitSettings &sett){
    ctx.title = sett.title;
    ctx.size = sett.size;
    ctx.setBackend(sett.ret);
    ctx.running = true;
    if(sett.size.x != 0 && sett.size.y != 0){
        ctx.size = sett.size;
    }    
    std::string wst = sett.fullscreen ? "fullscreen" : "windowed";
    astro::log("[GFX] initializing graphics '%s' | resolution: %dx%d | %s \n", RenderEngineType::name(ctx.render->type).c_str(), ctx.size.x, ctx.size.y, wst.c_str());
    auto suppR = ctx.render->isSupported();
    if(!suppR){
        astro::log("[GFX] fatal error: render engine '%s' is not supported\n", RenderEngineType::name(ctx.render->type).c_str());
        astro::Core::exit(1);
    }
    if (!glfwInit()){
        astro::log("[GFX] failed to start 'GLFW'");
        astro::Core::exit(1);
    }
    glfwWindowHint(GLFW_RESIZABLE, sett.resizeable ? GL_TRUE : GL_FALSE);
    auto initR = ctx.render->init();
    if (!initR){
        astro::log("[GFX] failure initializing renderer: %s\n", initR.msg.c_str());
        astro::Core::exit(1);
        return;
    }

    auto windowR = ctx.render->createWindow(ctx.title, ctx.size);
    if (!windowR){
        astro::log("[GFX] failure opening window: %s\n", windowR.msg.c_str());
        astro::Core::exit(1);
        return;
    }
    ctx.window = (GLFWwindow*)windowR.payload;
    __ASTRO_init_input(ctx.window);
    signal(SIGINT, ctrlC); // catch ctrl+c to start proper shut down (it fails sometimes though)
}

void astro::Gfx::update(){
    if(!ctx.running){
        return;
    }
    if(glfwWindowShouldClose(ctx.window)){
        end();
        return;
    }
    ctx.render->render();
    glfwPollEvents();
}

void astro::Gfx::end(){
    ctx.running = false;
    ctx.render->end();    
    __ASTRO_end_input();
    glfwDestroyWindow(ctx.window);
    glfwTerminate();
}

void astro::Gfx::onEnd(){
    ctx.running = false;
}

bool astro::Gfx::isRunning(){
    return ctx.running;
}