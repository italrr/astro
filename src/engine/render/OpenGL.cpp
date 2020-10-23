#include <GLFW/glfw3.h>
#include "../common/Result.hpp"
#include "OpenGL.hpp"
#include "../common/Log.hpp"

astro::Result astro::Gfx::RenderEngineOpenGL::init(){
    if (!glfwInit()){
        return astro::Result(ResultType::Failure, "failed to start 'GLFW': continuing headless...");
    }
    return astro::Result(ResultType::Success);
}

astro::Result astro::Gfx::RenderEngineOpenGL::end(){
    glfwMakeContextCurrent(NULL);
    return astro::Result(ResultType::Success);
}

astro::Result astro::Gfx::RenderEngineOpenGL::createWindow(const std::string &title, Vec2<int> size){
    GLFWwindow* window;
    window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
    if (!window){
        return astro::Result(ResultType::Failure, "failed to open window");
    }
    glfwMakeContextCurrent(window);
    return astro::Result(ResultType::Success, window);
}

astro::Result astro::Gfx::RenderEngineOpenGL::isSupported(){
    return astro::Result(ResultType::Success);
}