#include "glad.h"
#include <GLFW/glfw3.h>

#include "../common/Result.hpp"
#include "../common/Log.hpp"

#include "OpenGL.hpp"

astro::Result astro::Gfx::RenderEngineOpenGL::init(){
    return astro::Result(ResultType::Success);
}

astro::Result astro::Gfx::RenderEngineOpenGL::end(){
    glfwMakeContextCurrent(NULL);
    return astro::Result(ResultType::Success);
}

astro::Result astro::Gfx::RenderEngineOpenGL::createWindow(const std::string &title, Vec2<int> size){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window;
    window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
    if (!window){
        return astro::Result(ResultType::Failure, "failed to open window");
    }
    this->window = window;
    glfwMakeContextCurrent(window);
    gladLoadGL();
    astro::log("[GFX] GPU OpenGL version: %s\n", glGetString(GL_VERSION));
    readjustViewport(astro::Vec2<int>(0), size);
    return astro::Result(ResultType::Success, window);
}

astro::Result astro::Gfx::RenderEngineOpenGL::isSupported(){
    return astro::Result(ResultType::Success);
}

int astro::Gfx::RenderEngineOpenGL::render(){
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(static_cast<GLFWwindow*>(window));
    return 0;
}   

void astro::Gfx::RenderEngineOpenGL::readjustViewport(const astro::Vec2<int> &origin, const astro::Vec2<int> &size){
    glViewport(origin.x, origin.y, size.x, size.y);

    return;
}