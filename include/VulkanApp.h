#ifndef VULKANAPP_H
#define VULKANAPP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class VulkanApp {
    public:
        VulkanApp();

        ~VulkanApp();

        void run();

        std::vector<VkExtensionProperties> getAllSupportedExtensions();

    private:
        void initGLFW();

        void initVulkan();

        GLFWwindow* glfwWindow;
        VkInstance vulkanInstance;
};

#endif