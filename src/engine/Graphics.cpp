#include <GLFW/glfw3.h>
#include <signal.h>
#include <stdarg.h>
#include <algorithm>

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
    void readjust(){
        if(render == NULL){
            return;
        }
        render->readjustViewport(astro::Vec2<int>(0), size);
    }
    ~_Gfx(){
        if(this->render != NULL){
            return;
        }
        delete this->render;
    }
};

static _Gfx ctx = _Gfx();


std::shared_ptr<astro::Gfx::RenderLayer> astro::Gfx::RenderEngine::addRenderLayer(astro::Gfx::RenderLayerType type, const std::string &tag, int order){
    auto rt = std::shared_ptr<astro::Gfx::RenderLayer>(new astro::Gfx::RenderLayer());
    rt->type = type;
    rt->tag = tag;
    rt->order = order == -1 ? layers.size() : order;    
    layers.push_back(rt);
    std::sort(layers.begin(), layers.end(), [](std::shared_ptr<astro::Gfx::RenderLayer> a, std::shared_ptr<astro::Gfx::RenderLayer> b){
        return a->order > b->order;
    });
    return rt;
}

static void frameBufferResize(GLFWwindow* window, int width, int height){
    ctx.size = astro::Vec2<int>(width, height);
    ctx.readjust();
}

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

astro::Gfx::RenderEngine *astro::Gfx::getRenderEngine(){
    return ctx.render;
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
    astro::log("[GFX] init '%s' | res %dx%d | %s | %s \n", RenderEngineType::name(ctx.render->type).c_str(), ctx.size.x, ctx.size.y, Core::CurrentPlatform().c_str(), wst.c_str());
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
        astro::log("[GFX] failure initializing renderer: %s\n", initR->msg.c_str());
        astro::Core::exit(1);
        return;
    }

    auto windowR = ctx.render->createWindow(ctx.title, ctx.size);
    if (!windowR){
        astro::log("[GFX] failure opening window: %s\n", windowR->msg.c_str());
        astro::Core::exit(1);
        return;
    }
    ctx.window = (GLFWwindow*)windowR->ref;
    __ASTRO_init_input(ctx.window);
    glfwSetFramebufferSizeCallback(ctx.window, frameBufferResize);  
    signal(SIGINT, ctrlC); // catch ctrl+c to start proper shut down (it fails sometimes though)


    // add layers
    ctx.render->addRenderLayer(astro::Gfx::RenderLayerType::T_3D, "3dworld", 0);
    ctx.render->addRenderLayer(astro::Gfx::RenderLayerType::T_2D, "ui", 1);
    ctx.render->addRenderLayer(astro::Gfx::RenderLayerType::T_2D, "console", 2);
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

std::shared_ptr<astro::Result> astro::Gfx::RObj2DPrimitive::init(const std::vector<float> &verts, unsigned int nverts, unsigned int strides, bool textured){
    auto result = astro::makeResult(astro::ResultType::Waiting);
    this->vertices = verts;
    this->indices = indices;
    this->nverts = nverts;
    this->strides = strides;
    result->job = astro::spawn([&, result, textured](astro::Job &jctx){
        auto jgfx = astro::findJob({"astro_gfx"});
        if(jgfx.get() == NULL){
            result->setFailure(astro::String::format("gfx job not found: failed to generate primitive"));
            return;
        }
        jgfx->addBacklog([&, result, textured](astro::Job &jctx){
            auto ren = astro::Gfx::getRenderEngine();
            auto r =  ctx.render->generatePrimVertexBuffer(this->vertices, this->nverts, this->strides, textured);
            if(r->val == ResultType::Success){
                r->payload->reset();
                r->payload->read(&vbo, sizeof(vbo));
                r->payload->read(&vao, sizeof(vao));
            }
            result->set(r->val, r->msg);
        });

        
    }, true, false, true);

    return result;
}

void astro::Gfx::RObj2DPrimitive::render(){
    ctx.render->renderPrimVertBuffer(this);
}


astro::Gfx::Pipeline::Pipeline(const Pipeline &pip){
    astro::log("Gfx::Pipeline cannot be copied\n");
}

astro::Gfx::Pipeline::Pipeline(){
    lastId = 0;
}


void astro::Gfx::Camera::init(astro::Gfx::RenderEngine *renderer){
    this->renderer = renderer;
    this->yaw = -90.0f;
    this->pitch = 0.0f;
    this->up = astro::Vec3<float>(0.0f, 1.0f, 0.0f);
    this->worldUp = this->up;
    this->position = astro::Vec3<float>(0.0f, 0.0f, 0.0f);
    this->front = astro::Vec3<float>(0.0f, 0.0f, -1.0f);
    update();
}

void astro::Gfx::Camera::setPosition(const astro::Vec3<float> &pos){
    this->position = pos;
}

void astro::Gfx::Camera::setFront(const astro::Vec3<float> &front){
    this->front = front;
    this->update();
}

void astro::Gfx::Camera::update(){
    front.x = Math::cos(Math::rads(yaw)) * Math::cos(Math::rads(pitch));
    front.y = Math::sin(Math::rads(pitch));
    front.z = Math::sin(Math::rads(yaw)) * Math::cos(Math::rads(pitch));
    front = front.normalize();
    right = front.cross(this->worldUp).normalize();  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = right.cross(front).normalize();
}

void astro::Gfx::Camera::setUp(const astro::Vec3<float> &up){
    this->worldUp = up;
    this->update();
}

astro::Mat<4, 4, float> astro::Gfx::Camera::getView(){
    return Math::lookAt(position, position + front, up);
}

void astro::Gfx::Pipeline::init(astro::Gfx::RenderEngine *renderer, float fov, float nearp, float farp){
    std::unique_lock<std::mutex> lk(accesMutex);    
    this->renderer = renderer;
    this->camera.init(this->renderer);
    this->projection = astro::Math::perspective(fov, (float)this->renderer->size.x / (float)this->renderer->size.y, nearp, farp);
    lk.unlock();
}

void astro::Gfx::Pipeline::add(const std::shared_ptr<astro::Gfx::RenderObject> &obj){
    std::unique_lock<std::mutex> lk(accesMutex);
    unsigned int nId = ++lastId;
    obj->rObjId = nId;
    this->objects[nId] = obj;
    lk.unlock();
}   

void astro::Gfx::Pipeline::clear(){
    std::unique_lock<std::mutex> lk(accesMutex);
    objects.clear();
    lk.unlock();
}

void astro::Gfx::Pipeline::remove(int id){
    std::unique_lock<std::mutex> lk(accesMutex);
    auto it = objects.find(id);
    if(it == objects.end()){
        lk.unlock();
        return;
    }
    objects.erase(id);
    lk.unlock();
}

void astro::Gfx::Pipeline::remove(const std::shared_ptr<astro::Gfx::RenderObject> &obj){
    remove(obj->rObjId);
}

void astro::Gfx::Pipeline::render(){
    std::unique_lock<std::mutex> lk(accesMutex);
    for(auto &it : objects){
        auto &obj = it.second;
            
        obj->transform->shAttrs.clear();

        // model, view, projection
        obj->transform->shAttrs["model"] = std::make_shared<astro::Gfx::ShaderAttrMat4>(astro::Gfx::ShaderAttrMat4(obj->transform->model, "model"));
        obj->transform->shAttrs["view"] = std::make_shared<astro::Gfx::ShaderAttrMat4>(astro::Gfx::ShaderAttrMat4(camera.getView(), "view"));
        obj->transform->shAttrs["projection"] = std::make_shared<astro::Gfx::ShaderAttrMat4>(astro::Gfx::ShaderAttrMat4(projection, "projection"));

        // light
        int plightn = 0;
        int slightn = 0;
        int dirlightn = 0;
        for(int i = 0; i < lights.size(); ++i){
            switch(lights[i]->type){
                case LightType::DIRECTIONAL: {
                    auto light = std::static_pointer_cast<astro::Gfx::DirLight>(lights[i]);
                    std::string index = astro::String::format("dirLights[%i].", dirlightn);
                    obj->transform->shAttrs[index+"direction"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->direction));                    
                    obj->transform->shAttrs[index+"ambient"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->ambient));
                    obj->transform->shAttrs[index+"diffuse"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->diffuse));
                    obj->transform->shAttrs[index+"specular"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->specular));
                    ++dirlightn;
                } break;
                case LightType::SPOT: {
                    auto light =  std::static_pointer_cast<astro::Gfx::SpotLight>(lights[i]);
                    std::string index = astro::String::format("spotLights[%i].", slightn);
                    obj->transform->shAttrs[index+"position"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(camera.position));                    
                    obj->transform->shAttrs[index+"direction"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(camera.front));   

                    obj->transform->shAttrs[index+"ambient"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->ambient));
                    obj->transform->shAttrs[index+"diffuse"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->diffuse));
                    obj->transform->shAttrs[index+"specular"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->specular));

                    obj->transform->shAttrs[index+"constant"] = std::make_shared<astro::Gfx::ShaderAttrFloat>(astro::Gfx::ShaderAttrFloat(light->constant));
                    obj->transform->shAttrs[index+"linear"] = std::make_shared<astro::Gfx::ShaderAttrFloat>(astro::Gfx::ShaderAttrFloat(light->linear));
                    obj->transform->shAttrs[index+"quadratic"] = std::make_shared<astro::Gfx::ShaderAttrFloat>(astro::Gfx::ShaderAttrFloat(light->quadratic));
                    obj->transform->shAttrs[index+"cutOff"] = std::make_shared<astro::Gfx::ShaderAttrFloat>(astro::Gfx::ShaderAttrFloat(light->cutOff));
                    obj->transform->shAttrs[index+"outerCutOff"] = std::make_shared<astro::Gfx::ShaderAttrFloat>(astro::Gfx::ShaderAttrFloat(light->outerCutOff));
                    ++slightn;
                } break;                
                case LightType::POINT: {
                    auto light =  std::static_pointer_cast<astro::Gfx::PointLight>(lights[i]);
                    std::string index = astro::String::format("pointLights[%i].", plightn);
                    obj->transform->shAttrs[index+"position"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->position));                    
                    obj->transform->shAttrs[index+"ambient"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->ambient));
                    obj->transform->shAttrs[index+"diffuse"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->diffuse));
                    obj->transform->shAttrs[index+"specular"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(light->specular));

                    obj->transform->shAttrs[index+"constant"] = std::make_shared<astro::Gfx::ShaderAttrFloat>(astro::Gfx::ShaderAttrFloat(light->constant));
                    obj->transform->shAttrs[index+"linear"] = std::make_shared<astro::Gfx::ShaderAttrFloat>(astro::Gfx::ShaderAttrFloat(light->linear));
                    obj->transform->shAttrs[index+"quadratic"] = std::make_shared<astro::Gfx::ShaderAttrFloat>(astro::Gfx::ShaderAttrFloat(light->quadratic));
                    ++plightn;
                } break;
            }
        }

        obj->transform->shAttrs["n_point_lights"] = std::make_shared<astro::Gfx::ShaderAttrInt>(astro::Gfx::ShaderAttrInt(plightn));
        obj->transform->shAttrs["n_spot_lights"] = std::make_shared<astro::Gfx::ShaderAttrInt>(astro::Gfx::ShaderAttrInt(slightn));
        obj->transform->shAttrs["n_dir_lights"] = std::make_shared<astro::Gfx::ShaderAttrInt>(astro::Gfx::ShaderAttrInt(dirlightn));

        // viewpos
        obj->transform->shAttrs["viewPos"] = std::make_shared<astro::Gfx::ShaderAttrVec3>(astro::Gfx::ShaderAttrVec3(camera.position, "viewPos"));

        // material			
        // obj->transform->shAttrs["material.diffuse"] = std::make_shared<astro::Gfx::ShaderAttrInt>(astro::Gfx::ShaderAttrInt(0, "material.diffuse"));
        // obj->transform->shAttrs["material.specular"] = std::make_shared<astro::Gfx::ShaderAttrInt>(astro::Gfx::ShaderAttrInt(1, "material.specular"));
        obj->transform->shAttrs["material.shininess"] = std::make_shared<astro::Gfx::ShaderAttrFloat>(astro::Gfx::ShaderAttrFloat(obj->transform->material.shininess, "material.shininess"));
        int diff = 0;
        int spec = 0;
        int norm = 0;
        int height = 0;
        //String::format ("material.diffuse", diff++)
        if(obj->transform->textures.size() > 0){
            for(int i = 0; i < obj->transform->textures.size(); ++i){
                auto &tex = obj->transform->textures[i];
                switch(tex.role){
                    case TextureRole::DIFFUSE: {
                        obj->transform->shAttrs["material.diffuse"] = std::make_shared<astro::Gfx::ShaderAttrInt>(astro::Gfx::ShaderAttrInt(i));
                    } break;
                    case TextureRole::SPECULAR: {
                        obj->transform->shAttrs["material.specular"] = std::make_shared<astro::Gfx::ShaderAttrInt>(astro::Gfx::ShaderAttrInt(i));
                    } break;  
                    case TextureRole::NORMAL: {
                        obj->transform->shAttrs["material.normal"] = std::make_shared<astro::Gfx::ShaderAttrInt>(astro::Gfx::ShaderAttrInt(i));
                    } break;
                    case TextureRole::HEIGHT: {
                        obj->transform->shAttrs["material.height"] = std::make_shared<astro::Gfx::ShaderAttrInt>(astro::Gfx::ShaderAttrInt(i));
                    } break;                    
                }
            }
        }
        


        // push it to render it
        renderer->objects.push_back(obj);
    }
    lk.unlock();

}