#ifndef VULKANENGINE_H
#define VULKANENGINE_H

#include "VulkanInclude.h"

#include "VulkanInstance.h"
#include "VulkanDisplay.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanRenderSyncObjects.h"

#include <vector>
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

#include <fstream>

#include <memory>

class VulkanEngine {
    public:
        VulkanEngine();

        ~VulkanEngine();

        //set the current engine instance. setting this will recreate *everything* else, so be careful. also calls VulkanInstance::create() even if you already did.
        void setInstance(std::shared_ptr<VulkanInstance> instance);

        //set the current engine display. setting this will recreate *everything* else (except for the instance), so be careful. also calls VulkanDisplay::create(VulkanInstance instance) even if you already did.
        void setDisplay(std::shared_ptr<VulkanDisplay> display);

        //set the current engine device. setting this will recreate *everything* else (except for the instance and display), so be careful. also calls VulkanDevice::create(VulkanInstance instance, VulkanDisplay display) even if you already did.
        void setDevice(std::shared_ptr<VulkanDevice> device);

        //set the current engine swapchain. setting this will recreate *everything* else (except for the instance and display and device), so be careful. also calls VulkanSwapchain::create(VulkanInstance instance, VulkanDisplay display, VulkanDevice device) even if you already did.
        void setSwapchain(std::shared_ptr<VulkanSwapchain> swapchain);

        //set the current engine pipeline. setting this will recreate *everything* else (except for the instance and display and device), so be careful. also calls VulkanGraphicsPipeline::create(VulkanDevice device, VulkanSwapchain swapchain) even if you already did.
        void setGraphicsPipeline(std::shared_ptr<VulkanGraphicsPipeline> pipeline);

        //set the current engine sync objects. setting this will recreate *everything* else (except for the instance and display and device and swapchain), so be careful. also calls VulkanRenderSyncObjects::create(VulkanDevice device, VulkanSwapchain swapchain) even if you already did.
        void setSyncObjects(std::shared_ptr<VulkanRenderSyncObjects> syncObjects);

        std::shared_ptr<VulkanDisplay> getDisplay();

        std::shared_ptr<VulkanInstance> getInstance();

        std::shared_ptr<VulkanDevice> getDevice();

        std::shared_ptr<VulkanSwapchain> getSwapchain();

        std::shared_ptr<VulkanGraphicsPipeline> getGraphicsPipeline();

        std::shared_ptr<VulkanRenderSyncObjects> getSyncObjects();

        void recreateSwapchain();

        //helper functions
        static void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, std::shared_ptr<VulkanDevice>  device);
        
        static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, std::shared_ptr<VulkanDevice>  device);

        static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, std::shared_ptr<VulkanDevice> device);

        static VkCommandBuffer beginSingleTimeCommands(std::shared_ptr<VulkanDevice> device);

        static void endSingleTimeCommands(VkCommandBuffer commandBuffer, std::shared_ptr<VulkanDevice> device);

        static void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, std::shared_ptr<VulkanDevice> device);

        static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, std::shared_ptr<VulkanDevice> device);

        static void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, std::shared_ptr<VulkanDevice> device);

        static VkImageView createImageView(VkImage image, VkFormat format, std::shared_ptr<VulkanDevice> device);

    private:

        std::shared_ptr<VulkanInstance> vkInstance;
        std::shared_ptr<VulkanDisplay> vkDisplay;
        std::shared_ptr<VulkanDevice> vkDevice;
        std::shared_ptr<VulkanSwapchain> vkSwapchain;
        std::shared_ptr<VulkanGraphicsPipeline> vkPipeline;
        std::shared_ptr<VulkanRenderSyncObjects> vkSyncObjects;

        bool hasInstance = false;
        bool hasDisplay = false;
        bool hasDevice = false;
        bool hasSwapchain = false;

        bool hasPipeline = false;
        bool hasSyncObjects = false;
};

#endif