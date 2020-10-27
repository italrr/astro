#include <GLFW/glfw3.h>
#include <signal.h>

#include "common/3rdparty/Jzon.hpp"
#include "common/Log.hpp"
#include "common/Job.hpp"
#include "Graphics.hpp"
#include "Core.hpp"
#include "render/Vulkan.hpp"
#include "render/OpenGL.hpp"

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
    RenderInitSettings sett = RenderInitSettings();
    sett.title = title;
    sett.size = size;
    init(sett);
}

void astro::Gfx::init(const std::string &title, astro::Gfx::RenderInitSettings &sett){
    sett.title = title;
    init(sett);
}

void astro::Gfx::init(const std::string &title, RenderEngineType::RenderEngineType ret){    
    RenderInitSettings sett = RenderInitSettings();
    sett.title = title;
    sett.ret = ret;
    init(sett);    
}

void astro::Gfx::init(RenderInitSettings &sett){
    auto sfile = astro::Core::getSettingsFile();
    ctx.title = sett.title;
    ctx.size = sfile.size;
    ctx.setBackend(sett.ret == astro::Gfx::RenderEngineType::Undefined ? astro::Gfx::RenderEngineType::value(sfile.backend) : sett.ret);
    ctx.running = true;
    if(sett.size.x != 0 && sett.size.y != 0){
        ctx.size = sett.size;
    }    
    astro::log("astro ~> *\n");
    astro::log("initializing graphics '%s' | resolution: %dx%d\n", RenderEngineType::name(ctx.render->type).c_str(), ctx.size.x, ctx.size.y);
    auto suppR = ctx.render->isSupported();
    if(!suppR){
        astro::log("fatal error: render engine '%s' is not supported\n", RenderEngineType::name(ctx.render->type).c_str());
        astro::Core::exit(1);
    }

    auto initR = ctx.render->init();
    if (!initR){
        astro::log("failure initializing renderer: %s\n", initR.msg.c_str());
        astro::Core::exit(1);
        return;
    }

    auto windowR = ctx.render->createWindow(ctx.title, ctx.size);
    if (!windowR){
        astro::log("failure opening window: %s\n", windowR.msg.c_str());
        astro::Core::exit(1);
        return;
    }
    ctx.window = (GLFWwindow*)windowR.payload;

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
    glfwSwapBuffers(ctx.window);
    glfwPollEvents();
}

void astro::Gfx::end(){
    ctx.running = false;
    ctx.render->end();    
    glfwDestroyWindow(ctx.window);
    glfwTerminate();
}

void astro::Gfx::onEnd(){
    ctx.running = false;
}

bool astro::Gfx::isRunning(){
    return ctx.running;
}