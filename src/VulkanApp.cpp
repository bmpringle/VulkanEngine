#include "VulkanApp.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

VulkanApp::VulkanApp() {
    initGLFW();
    initVulkan();
};

VulkanApp::~VulkanApp() {
    //destroy window object
    glfwDestroyWindow(glfwWindow);

    //terminate glfw
    glfwTerminate();

    //destroy vulkan instance (should be last vulkan object to be destroyed)
    vkDestroyInstance(vulkanInstance, nullptr);
};

void VulkanApp::run() {
    //main loop
    while(!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();
    } 
};

std::vector<VkExtensionProperties> VulkanApp::getAllSupportedExtensions() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions = std::vector<VkExtensionProperties>(extensionCount);

  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  return extensions;
}

void VulkanApp::initGLFW() {
    //init glfw
    glfwInit();

    //we're using vulkan, disable the default api
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    //requires special behavior to use with vulkan, disable for now.
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    //create window
    glfwWindow = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
}

void VulkanApp::initVulkan() {
    //query what extensions are available then list them
    std::cout << "Available Vulkan Extensions:" << std::endl;

    std::vector<VkExtensionProperties> extensions = getAllSupportedExtensions();

    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }

    //Create VkApplicationInfo struct and fill with data
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    //Create VkInstanceCreateInfo struct and fill with data
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    //get extensions required by glfw to render with vulkan
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    //attempt to create VkInstance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("couldn't create vkinstance: " + std::to_string(result));
    }else {
        std::cout << "successfully created VkInstance!" << std::endl;
    }
}