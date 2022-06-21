#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#include "VulkanInclude.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"
#include "VulkanInstance.h"
#include "VulkanDisplay.h"

#include <vector>
#include <set>

#include <memory>

class VulkanDevice {
    public:
        VulkanDevice();

        void destroyDevice();

        void create(std::shared_ptr<VulkanInstance> instance, std::shared_ptr<VulkanDisplay> display);

        void addDeviceExtension(const char* deviceExtension);

        bool isCreated();

        //public access only used AFTER creation.
        SwapChainSupportDetails getDeviceSwapChainSupport(std::shared_ptr<VulkanDisplay> display);

        VkDevice& getInternalLogicalDevice();

        VkPhysicalDevice& getInternalPhysicalDevice();

        VkCommandPool& getInternalCommandPool();

        QueueFamilyIndices getDeviceQueueFamilies(std::shared_ptr<VulkanDisplay> display);

        VkQueue& getInternalGraphicsQueue();

        VkQueue& getInternalPresentQueue();
    private:
        void createPhysicalDevice(std::shared_ptr<VulkanInstance> instance, std::shared_ptr<VulkanDisplay> display);

        void createLogicalDevice(std::shared_ptr<VulkanInstance> instance);

        void createCommandPool();

        static QueueFamilyIndices getDeviceQueueFamilies(VkPhysicalDevice pDevice, std::shared_ptr<VulkanDisplay> display);
        
        bool canDeviceBeUsed(VkPhysicalDevice pDevice, std::shared_ptr<VulkanDisplay> display);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        SwapChainSupportDetails getDeviceSwapChainSupport(VkPhysicalDevice device, std::shared_ptr<VulkanDisplay> display);

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        QueueFamilyIndices indices;

        VkDevice logicalDevice;

        VkQueue graphicsQueue;

        VkQueue presentQueue;

        VkCommandPool graphicsCommandPool;

        std::vector<const char*> deviceExtensions;

        bool hasBeenCreated = false;
};

#endif