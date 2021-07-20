#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#include "VulkanInclude.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"
#include "VulkanInstance.h"
#include "VulkanDisplay.h"

#include <vector>
#include <set>

class VulkanDevice {
    public:
        VulkanDevice();

        void destroyDevice();

        void create(VulkanInstance instance, VulkanDisplay display);

        void addDeviceExtension(const char* deviceExtension);

        bool isCreated();
    private:
        void createPhysicalDevice(VulkanInstance instance, VulkanDisplay display);

        void createLogicalDevice(VulkanInstance instance);

        static QueueFamilyIndices getDeviceQueueFamilies(VkPhysicalDevice pDevice, VulkanDisplay display);
        
        bool canDeviceBeUsed(VkPhysicalDevice pDevice, VulkanDisplay display);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        SwapChainSupportDetails getDeviceSwapChainSupport(VkPhysicalDevice device, VulkanDisplay display);

        VkPhysicalDevice physicalDevice;

        QueueFamilyIndices indices;

        VkDevice logicalDevice;

        VkQueue graphicsQueue;

        VkQueue presentQueue;

        std::vector<const char*> deviceExtensions;

        bool hasBeenCreated = false;
};

#endif