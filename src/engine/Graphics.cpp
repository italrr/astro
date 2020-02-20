#include "GLFW/glfw3.h"

#include "Graphics.hpp"
#include "Log.hpp"

struct _Gfx {
    std::string title;
    float timescale;
    bool headless;
    bool running;
    astro::Vec2<int> size;
    GLFWwindow *window;
    _Gfx(){
        this->title = "astro";
        this->timescale = 1.0f;
        this->headless = false;
        this->running = false;
        this->size = astro::Vec2<int>(1920, 1080);
        this->window = NULL;
    }
};

static _Gfx ctx = _Gfx();

void astro::Gfx::init(const std::string &title, astro::Vec2<int> size){
    ctx.title = title;
    ctx.size = size;
    astro::log("[astro engine started]\n");
    astro::log("starting graphics %dx%d\n", size.x, size.y);

    if (!glfwInit()){
        astro::log("failed to start 'GLFW': continuing headless...\n");
        ctx.headless = true;
        return;
    }

    ctx.running = true;
}

void astro::Gfx::update(){

}

void astro::Gfx::end(){
    ctx.running = false;
}

bool astro::Gfx::isRunning(){
    return ctx.running;
}

    

    // /* Initialize the library */


    // /* Create a windowed mode window and its OpenGL context */
    // window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
    // if (!window)
    // {
    //     glfwTerminate();
    //     return -1;
    // }

    // /* Make the window's context current */
    // glfwMakeContextCurrent(window);

    // /* Loop until the user closes the window */
    // while (!glfwWindowShouldClose(window))
    // {
    //     /* Render here */
    //     glClear(GL_COLOR_BUFFER_BIT);

    //     /* Swap front and back buffers */
    //     glfwSwapBuffers(window);

    //     /* Poll for and process events */
    //     glfwPollEvents();
    // }

    // glfwTerminate();