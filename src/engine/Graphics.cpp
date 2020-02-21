#include <GLFW/glfw3.h>

#include "common/Log.hpp"
#include "common/Job.hpp"
#include "Graphics.hpp"

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
        this->size = astro::Vec2<int>(1920, 1080);
        this->window = NULL;
        this->render = new astro::Gfx::RenderEngineVulkan();
    }
    ~_Gfx(){
        delete this->render;
    }
};

static _Gfx ctx = _Gfx();

void astro::Gfx::init(const std::string &title, astro::Vec2<int> size){
    ctx.title = title;
    ctx.size = size;
    ctx.running = true;
    astro::log("astro ~> *\n");
    astro::log("Initializing graphics '"+RenderEngineType::name(ctx.render->type)+"' | resolution: %dx%d\n", size.x, size.y);

    auto suppR = ctx.render->isSupported();
    if(!suppR){
        astro::log("Render '"+RenderEngineType::name(ctx.render->type)+"' is not supported : continuing headless...\n");
        ctx.headless = true;
        return;   
    }

    auto initR = ctx.render->init();
    if (!initR){
        astro::log(initR.msg+"\n");
        ctx.headless = true;
        return;
    }

    auto windowR = ctx.render->createWindow(ctx.title, ctx.size);
    if (!windowR){
        astro::log(windowR.msg+"\n");
        ctx.headless = true;
        return;
    }
}

void astro::Gfx::update(){
    glfwPollEvents();
}

void astro::Gfx::end(){
    ctx.render->end();
    glfwDestroyWindow(ctx.window);
    glfwTerminate();
    ctx.running = false;
}


void astro::Gfx::onEnd(){
    ctx.running = false;
}

/*
    Returns whether graphics is running.
*/
bool astro::Gfx::isRunning(){
    return ctx.running;
}