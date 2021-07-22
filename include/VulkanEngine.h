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

class VulkanEngine {
    public:
        VulkanEngine();

        ~VulkanEngine();

        //set the current engine instance. setting this will recreate *everything* else, so be careful. also calls VulkanInstance::create() even if you already did.
        void setInstance(VulkanInstance instance);

        //set the current engine display. setting this will recreate *everything* else (except for the instance), so be careful. also calls VulkanDisplay::create(VulkanInstance instance) even if you already did.
        void setDisplay(VulkanDisplay display);

        //set the current engine device. setting this will recreate *everything* else (except for the instance and display), so be careful. also calls VulkanDevice::create(VulkanInstance instance, VulkanDisplay display) even if you already did.
        void setDevice(VulkanDevice device);

        //set the current engine swapchain. setting this will recreate *everything* else (except for the instance and display and device), so be careful. also calls VulkanSwapchain::create(VulkanInstance instance, VulkanDisplay display, VulkanDevice device) even if you already did.
        void setSwapchain(VulkanSwapchain swapchain);

        //set the current engine pipeline. setting this will recreate *everything* else (except for the instance and display and device), so be careful. also calls VulkanGraphicsPipeline::create(VulkanDevice device, VulkanSwapchain swapchain) even if you already did.
        void setGraphicsPipeline(VulkanGraphicsPipeline pipeline);

        //set the current engine sync objects. setting this will recreate *everything* else (except for the instance and display and device and swapchain), so be careful. also calls VulkanRenderSyncObjects::create(VulkanDevice device, VulkanSwapchain swapchain) even if you already did.
        void setSyncObjects(VulkanRenderSyncObjects syncObjects);

        VulkanDisplay getDisplay();

        VulkanInstance getInstance();

        VulkanDevice getDevice();

        VulkanSwapchain getSwapchain();

        VulkanGraphicsPipeline getGraphicsPipeline();

        VulkanRenderSyncObjects getSyncObjects();

        //put in your render loop, giving control over to the engine to do things like make draw calls you submitted etc and poll window events.
        void engineLoop();
    
    private:
        void recordCommandBuffers();

        void drawFrame();

        VulkanInstance vkInstance;
        VulkanDisplay vkDisplay;
        VulkanDevice vkDevice;
        VulkanSwapchain vkSwapchain;
        VulkanGraphicsPipeline vkPipeline;
        VulkanRenderSyncObjects vkSyncObjects;

        bool hasInstance = false;
        bool hasDisplay = false;
        bool hasDevice = false;
        bool hasSwapchain = false;

        bool hasPipeline = false;
        bool hasSyncObjects = false;

        size_t currentFrame = 0;
};

#endif