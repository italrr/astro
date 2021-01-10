#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string.h>

#include "../common/Log.hpp"

#include "Vulkan.hpp"

struct _VulkanCtx {
    VkInstance instance;
    VkSurfaceKHR surface;

    // functions
    PFN_vkCreateInstance pfnCreateInstance;
    PFN_vkDestroyInstance pfnDestroyInstance;
    PFN_vkDestroySurfaceKHR pfnDestroySurfaceKHR;
    _VulkanCtx(){
        instance = NULL;
        surface = NULL;
    }
    void queryCoreFunctions(){
        this->pfnCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(NULL, "vkCreateInstance");
        this->pfnDestroyInstance = (PFN_vkDestroyInstance)glfwGetInstanceProcAddress(NULL, "vkDestroyInstance");
        this->pfnDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)glfwGetInstanceProcAddress(NULL, "vkDestroySurfaceKHR");
    }
};
static _VulkanCtx ctx = _VulkanCtx();

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineVulkan::init(){
    auto result = astro::makeResult(astro::ResultType::Success);

    // app info
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "astro | vulkan backend";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "astro";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;    

    VkInstanceCreateInfo createInfo = {};
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    ctx.queryCoreFunctions();

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

    if (ctx.pfnCreateInstance(&createInfo, NULL, &ctx.instance) != VK_SUCCESS) {
        result->setFailure("failed to create Vulkan instance");
        return result;
    }    

    return result;
}

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineVulkan::end(){
    if(ctx.surface != NULL){
        ctx.pfnDestroySurfaceKHR(ctx.instance, ctx.surface, NULL); 
    }
    if(ctx.instance != NULL){
        ctx.pfnDestroyInstance(ctx.instance, NULL); 
    }
    return astro::makeResult(astro::ResultType::Success);
}

std::shared_ptr<astro::Result> astro::Gfx::RenderEngineVulkan::createWindow(const std::string &title, Vec2<int> size){
    auto result = astro::makeResult(astro::ResultType::Success);
    GLFWwindow* window;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
    if (!window){
        result->setFailure("failed to open window");
        return result;
    }
    this->window = window;
    glfwCreateWindowSurface(ctx.instance, window, NULL, &ctx.surface);
    result->set((void*)this->window);
    return result;
}

std::shared_ptr<astro::Result>  astro::Gfx::RenderEngineVulkan::isSupported(){
    return astro::makeResult(ResultType::Success);
}


int astro::Gfx::RenderEngineVulkan::render(){

    return 0;
}