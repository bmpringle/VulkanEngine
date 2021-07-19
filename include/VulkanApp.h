#ifndef VULKANAPP_H
#define VULKANAPP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

class VulkanApp {
    public:
        VulkanApp();

        ~VulkanApp();

        void run();

        std::vector<VkExtensionProperties> getAllSupportedExtensions();

    private:
        void initGLFW();

        void initVulkan();

        void createInstance();

        void createSurface();

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createSwapChain();

        void createImageViews();

        void createGraphicsPipeline();

        bool checkValidationLayerSupport();

        bool canDeviceBeUsed(VkPhysicalDevice device);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        QueueFamilyIndices getDeviceQueueFamilies(VkPhysicalDevice device);

        SwapChainSupportDetails getDeviceSwapChainSupport(VkPhysicalDevice device);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        GLFWwindow* glfwWindow;

        VkInstance vulkanInstance;

        VkSurfaceKHR surface;

        //auto(magically) destroyed when vkInstance is destroyed
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        QueueFamilyIndices deviceFamilyIndices;

        VkDevice device;

        VkQueue graphicsQueue;

        VkQueue presentQueue;

        VkSwapchainKHR swapChain;

        std::vector<VkImage> swapChainImages;

        std::vector<VkImageView> swapChainImageViews;

        VkFormat swapChainImageFormat;

        VkExtent2D swapChainExtent;

        const std::vector<const char*> validationLayers;

        const std::vector<const char*> deviceExtensions;

};

#endif