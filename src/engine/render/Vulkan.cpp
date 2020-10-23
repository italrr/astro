#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string.h>
#include "Vulkan.hpp"
#include "../common/Log.hpp"

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

astro::Result astro::Gfx::RenderEngineVulkan::init(){
    if (!glfwInit()){
        return astro::Result(ResultType::Failure, "failed to start 'GLFW': continuing headless...");
    }

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
        return astro::Result(ResultType::Failure, "failed to create Vulkan instance");
    }    

    return astro::Result(ResultType::Success);
}

astro::Result astro::Gfx::RenderEngineVulkan::end(){
    if(ctx.surface != NULL){
        ctx.pfnDestroySurfaceKHR(ctx.instance, ctx.surface, NULL); 
    }
    if(ctx.instance != NULL){
        ctx.pfnDestroyInstance(ctx.instance, NULL); 
    }
    return astro::Result(ResultType::Success);
}

astro::Result astro::Gfx::RenderEngineVulkan::createWindow(const std::string &title, Vec2<int> size){
    GLFWwindow* window;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
    if (!window){
        return astro::Result(ResultType::Failure, "failed to open window");
    }
    glfwCreateWindowSurface(ctx.instance, window, NULL, &ctx.surface);
    return astro::Result(ResultType::Success, window);
}

astro::Result astro::Gfx::RenderEngineVulkan::isSupported(){
    return astro::Result(ResultType::Success);
}

