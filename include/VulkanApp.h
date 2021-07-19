#ifndef VULKANAPP_H
#define VULKANAPP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

#include <fstream>

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

        void createRenderPass();

        void createGraphicsPipeline();

        void createFramebuffers();

        void createCommandPool();

        void createCommandBuffers();

        void recordCommandBuffers();

        void createSyncObjects();

        void drawFrame();

        bool checkValidationLayerSupport();

        bool canDeviceBeUsed(VkPhysicalDevice device);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        static std::vector<char> readFile(const std::string& filename);

        QueueFamilyIndices getDeviceQueueFamilies(VkPhysicalDevice device);

        SwapChainSupportDetails getDeviceSwapChainSupport(VkPhysicalDevice device);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        VkShaderModule createShaderModule(const std::vector<char>& shaderCode);

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

        std::vector<VkFramebuffer> swapChainFramebuffers;

        std::vector<VkCommandBuffer> commandBuffers;

        VkFormat swapChainImageFormat;

        VkExtent2D swapChainExtent;

        VkRenderPass renderPass;

        VkPipelineLayout pipelineLayout;

        VkPipeline graphicsPipeline;

        VkCommandPool commandPool;

        std::vector<VkSemaphore> imageAvailableSemaphores;

        std::vector<VkSemaphore> renderFinishedSemaphores;

        std::vector<VkFence> inFlightFences;

        std::vector<VkFence> imagesInFlight;

        const std::vector<const char*> validationLayers;

        const std::vector<const char*> deviceExtensions;

        const int MAX_FRAMES_IN_FLIGHT = 2;

        size_t currentFrame = 0;

};

#endif