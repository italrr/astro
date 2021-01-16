#include "glad.h"
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdarg.h>

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
    this->size.x = size.x;
    this->size.y = size.y;
    if (!window){
        result->setFailure("failed to open window");
        return result;
    }
    this->window = window;
    glfwMakeContextCurrent(window);
    gladLoadGL();
    // glfwSwapInterval(0);
    astro::log("[GFX] GPU OpenGL version: %s\n", glGetString(GL_VERSION));
    readjustViewport(astro::Vec2<int>(0), size);
    glEnable(GL_DEPTH_TEST); 
    result->set((void*)window);
    return result;
}

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineOpenGL::isSupported(){
    return astro::makeResult(ResultType::Success);
}

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineOpenGL::generatePrimVertexBuffer(
                                                        const std::vector<float> &verts,
                                                        const std::vector<int> &indices,
                                                        bool incTex){
    auto result = astro::makeResult(ResultType::Success);
    float vert[verts.size()];
    for(int i = 0; i < verts.size(); ++i){
        vert[i] = verts[i];
    }
    int ind[indices.size()];
    for(int i = 0; i < indices.size(); ++i){
        ind[i] = indices[i];
    }              
    // gen buffers
    unsigned int vbo;
    unsigned int vao;
    unsigned int ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);  
    // glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW); // TODO: 'GL_STATIC_DRAW': allow to specify other types?
    
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind), ind, GL_STATIC_DRAW);

    // int ncomp = incTex ? 8 : 3;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // if(incTex){
    //     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, ncomp * sizeof(float), (void*)(3 * sizeof(float)));
    //     glEnableVertexAttribArray(1);
    //     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, ncomp * sizeof(float), (void*)(6 * sizeof(float)));
    //     glEnableVertexAttribArray(2);
    // }

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
    // copy ids to payload
    result->payload->write(&vbo, sizeof(unsigned int));
    result->payload->write(&vao, sizeof(unsigned int));
    result->payload->write(&ebo, sizeof(unsigned int));

    return result;
}

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineOpenGL::generateTexture2D(unsigned char *data, int w, int h){
    auto result = astro::makeResult(ResultType::Success);
    unsigned int texture;
    glGenTextures(1, &texture);  
    glBindTexture(GL_TEXTURE_2D, texture);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    result->payload->write(&texture, sizeof(texture));
    return result;
}

bool astro::Gfx::RenderEngineOpenGL::renderPrimVertBuffer(astro::Gfx::RenderObject *obj){

    auto prim = static_cast<RObj2DPrimitive*>(obj);
    // bind shader
    if(prim->transform->shader.get() != NULL){
        glUseProgram(prim->transform->shader->shaderId);
        if(prim->transform->shAttrs.size() > 0){
            for(auto &it : prim->transform->shAttrs){
                switch(it.second->type){
                    case ShaderAttrType::FLOAT: {
                        auto attrf = std::static_pointer_cast<astro::Gfx::ShaderAttrFloat>(it.second);
                        glUniform1f(glGetUniformLocation(prim->transform->shader->shaderId, it.first.c_str()), attrf->n);
                    } break;
                    case ShaderAttrType::INT: {
                        auto attri = std::static_pointer_cast<astro::Gfx::ShaderAttrInt>(it.second);
                        glUniform1i(glGetUniformLocation(prim->transform->shader->shaderId, it.first.c_str()), attri->n);
                    } break;                 
                    case ShaderAttrType::COLOR: {
                        auto attrc = std::static_pointer_cast<astro::Gfx::ShaderAttrColor>(it.second);
                        float v[4] = {attrc->color.r, attrc->color.g, attrc->color.b, attrc->color.a};
                        glUniform4fv(glGetUniformLocation(prim->transform->shader->shaderId, it.first.c_str()), 1, v);
                    } break;
                    case ShaderAttrType::VEC2: {
                        auto attrvec = std::static_pointer_cast<astro::Gfx::ShaderAttrVec2>(it.second);
                        float v[2] = {attrvec->vec.x, attrvec->vec.y};
                        glUniform2fv(glGetUniformLocation(prim->transform->shader->shaderId, it.first.c_str()), 1, v);
                    } break;     
                    case ShaderAttrType::VEC3: {
                        auto attrvec = std::static_pointer_cast<astro::Gfx::ShaderAttrVec3>(it.second);
                        float v[3] = {attrvec->vec.x, attrvec->vec.y, attrvec->vec.z};
                        glUniform3fv(glGetUniformLocation(prim->transform->shader->shaderId, it.first.c_str()), 1, v);
                    } break; 
                    case ShaderAttrType::MAT4: {
                        auto attrmat = std::static_pointer_cast<astro::Gfx::ShaderAttrMat4>(it.second);
                        unsigned int loc = glGetUniformLocation(prim->transform->shader->shaderId, it.first.c_str());
                        glUniformMatrix4fv(loc, 1, GL_FALSE, attrmat->mat.mat);                        
                    } break;                                                           
                    default: {
                        astro::log("[GFX] undefined shader type to apply '%s'\n", it.second->type);
                    } break;
                }
            }
        }
    }
    // bind texture
    if(prim->transform->texture.get() != NULL){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, prim->transform->texture->textureId);
    }
    if(prim->transform->texture2.get() != NULL){
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, prim->transform->texture2->textureId);
    }    
    glBindVertexArray(prim->vao);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    return true;
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
    char *buffer = new char[logLength];
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.03f, 0.0f, 0.07f, 1.0f);

    for(int i = 0; i < objects.size(); ++i){
        objects[i]->render();
    }

    glfwSwapBuffers(static_cast<GLFWwindow*>(window));
    objects.clear();
    return 0;
}   

void astro::Gfx::RenderEngineOpenGL::readjustViewport(const astro::Vec2<int> &origin, const astro::Vec2<int> &size){
    glViewport(origin.x, origin.y, size.x, size.y);
    return;
}