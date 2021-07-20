#ifndef VULKANENGINE_H
#define VULKANENGINE_H

#include "VulkanInclude.h"

#include "VulkanInstance.h"
#include "VulkanDisplay.h"
#include "VulkanDevice.h"

#include <vector>
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

#include <fstream>

//Instance object -- has VkInstance in it, deals with instance extensions etc.
//Display object -- has glfw window in it, deals with resizing/keypress etc events. also has khrsurface, deals with settings when creating it
//Device object -- has Physical Device and Logical Device in it. also has graphics and present queues. deals with physical gpu requirements for picking it and device extensions (when creating logical device).
//Swapchain object -- has Swapchain, Images, ImageViews, Framebuffers, Command Buffers, Image Format, Extent, Render Pass, Pipeline Layout, Graphics Pipeline,  
//Command Pool - is a command pool, so it manages Command Buffers

class VulkanEngine {
    public:
        VulkanEngine();

        ~VulkanEngine();

        //set the current engine instance. setting this will recreate *everything* else, so be careful. also calls VulkanInstance::create() even if you already did.
        void setInstance(VulkanInstance instance);

        //set the current engine display. setting this will recreate *everything* else (except for the instance), so be careful. also calls VulkanDisplay::create(VulkanInstance instance) even if you already did.
        void setDisplay(VulkanDisplay display);

        //set the current engine display. setting this will recreate *everything* else (except for the instance and display), so be careful. also calls VulkanDevice::create(VulkanInstance instance, VulkanDisplay display) even if you already did.
        void setDevice(VulkanDevice device);

        VulkanDisplay getDisplay();

        VulkanInstance getInstance();

        VulkanDevice getDevice();

        //put in your render loop, giving control over to the engine to do things like make draw calls you submitted etc and poll window events.
        void engineLoop();
    
    private:
        VulkanInstance vkInstance;
        VulkanDisplay vkDisplay;
        VulkanDevice vkDevice;

        bool hasInstance = false;
        bool hasDisplay = false;
        bool hasDevice = false;
};

#endif