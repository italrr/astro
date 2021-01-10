#include "glad.h"
#include <GLFW/glfw3.h>

#include "../common/Result.hpp"
#include "../common/Log.hpp"

#include "OpenGL.hpp"

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineOpenGL::init(){
    return astro::makeResult(ResultType::Success);
}

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineOpenGL::end(){
    glfwMakeContextCurrent(NULL);
    return astro::makeResult(ResultType::Success);
}

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineOpenGL::createWindow(const std::string &title, Vec2<int> size){
    auto result = astro::makeResult(ResultType::Success);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window;
    window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
    if (!window){
        result->setFailure("failed to open window");
        return result;
    }
    this->window = window;
    glfwMakeContextCurrent(window);
    gladLoadGL();
    astro::log("[GFX] GPU OpenGL version: %s\n", glGetString(GL_VERSION));
    readjustViewport(astro::Vec2<int>(0), size);
    result->set((void*)window);
    return result;
}

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineOpenGL::isSupported(){
    return astro::makeResult(ResultType::Success);
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