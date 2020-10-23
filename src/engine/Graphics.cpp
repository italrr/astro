#include <GLFW/glfw3.h>
#include <signal.h>

#include "common/3rdparty/Jzon.hpp"
#include "common/Log.hpp"
#include "common/Job.hpp"
#include "common/Tools.hpp"
#include "Graphics.hpp"
#include "Core.hpp"

#include "render/OpenGL.hpp"
#include "render/Vulkan.hpp"


struct _Gfx {
    std::string title;
    float timescale;
    bool headless;
    bool running;
    astro::Vec2<int> size;
    GLFWwindow *window;
    astro::Gfx::RenderEngine *render;
    _Gfx(){
        this->title = "astro";
        this->timescale = 1.0f;
        this->headless = false;
        this->running = false;
        this->size = astro::Vec2<int>(640, 480);
        this->window = NULL;
        this->render = NULL;
    }
    void setBackend(const std::string &name){
        if(astro::String::toLower(name) == "opengl"){
            this->render = new astro::Gfx::RenderEngineOpenGL();
        }else
        if(astro::String::toLower(name) == "vulkan"){
            this->render = new astro::Gfx::RenderEngineVulkan();
        }else{
            astro::log("failed to initialize graphics: render engine '%s' is undefined\n", name.c_str());
            astro::Core::exit(1);
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
    ctx.title = title;
    ctx.size = sfile.size;
    ctx.setBackend(sfile.backend);
    ctx.running = true;
    if(size.x != 0 && size.y != 0){
        ctx.size = size;
    }    
    astro::log("astro ~> *\n");
    astro::log("initializing graphics '%s' | resolution: %dx%d\n", RenderEngineType::name(ctx.render->type).c_str(), ctx.size.x, ctx.size.y);
    auto suppR = ctx.render->isSupported();
    if(!suppR){
        astro::log("fender '%s' is not supported : continuing headless...\n", RenderEngineType::name(ctx.render->type).c_str());
        ctx.headless = true;
        return;   
    }

    auto initR = ctx.render->init();
    if (!initR){
        astro::log("failure initializing renderer: %s\n", initR.msg.c_str());
        ctx.headless = true;
        return;
    }

    auto windowR = ctx.render->createWindow(ctx.title, ctx.size);
    if (!windowR){
        astro::log("failure opening window: %s\n", windowR.msg.c_str());
        ctx.headless = true;
        return;
    }
    ctx.window = (GLFWwindow*)windowR.payload;

    signal(SIGINT, ctrlC);
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