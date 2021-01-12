#include "glad.h"
#include <GLFW/glfw3.h>
#include <string.h>

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

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineOpenGL::compileShader(const std::string &vert, const std::string &frag){
    auto result = astro::makeResult(ResultType::Success);
    int shaderId;
    std::string str;
    const char *fragSrc = frag.c_str();
    const char *vertSrc = vert.c_str();
    // create shaders
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLint gResult = GL_FALSE;
    int logLength = 1024 * 5;
    char *buffer = new char[1024 * 5];
    memset(buffer, 0, logLength);
    // compile vertex
    glShaderSource(vertShader, 1, &vertSrc, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &gResult);
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
    memset(buffer, 0, logLength);
    glGetShaderInfoLog(vertShader, logLength, NULL, buffer);
    str = std::string(buffer);
    if(str.length() > 0) {
        result->setFailure(astro::String::format("compilation error: \n\n%s\n\n", str.c_str()));
    }
    str = "";
    // compile frag
    glShaderSource(fragShader, 1, &fragSrc, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &gResult);
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
    memset(buffer, 0, logLength);
    glGetShaderInfoLog(fragShader, logLength, NULL, buffer);
    str = std::string(buffer);
    if(str.length() > 0) {
        result->setFailure(astro::String::format("compilation error: \n\n%s\n\n", str.c_str()));
    }
    str = "";
    // put together
    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertShader);
    glAttachShader(shaderId, fragShader);
    glLinkProgram(shaderId);
    // check status
    glGetProgramiv(shaderId, GL_LINK_STATUS, &gResult);
    glGetProgramiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    memset(buffer, 0, logLength);
    glGetProgramInfoLog(shaderId, logLength, NULL, buffer);
    str = std::string(buffer);
    if(str.length() > 0) {
        result->setFailure(astro::String::format("compilation error: \n\n%s\n\n", str.c_str()));
        return result;
    }
    str = "";
    // clean
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    delete buffer;
    result->payload->write(&shaderId, sizeof(shaderId));
    return result;
}

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineOpenGL::deleteShader(int id){
    glDeleteProgram(id);
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