#include "VulkanEngine.h"

VulkanEngine::VulkanEngine() {

}

VulkanEngine::~VulkanEngine() {
    vkPipeline.destroyGraphicsPipeline(vkDevice);
    vkSwapchain.destroySwapchain(vkDevice);
    vkDevice.destroyDevice();
    vkDisplay.destroyDisplay(vkInstance);
    vkInstance.destroyInstance();
}

void VulkanEngine::setInstance(VulkanInstance instance) {
    if(hasPipeline) {
        vkPipeline.destroyGraphicsPipeline(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain.destroySwapchain(vkDevice);
    }

    if(hasDevice) {
        vkDevice.destroyDevice();
    }

    if(hasDisplay) {
        vkDisplay.destroyDisplay(vkInstance);
    }

    if(hasInstance) {
        vkInstance.destroyInstance();
    }

    vkInstance = instance;
    vkInstance.create();

    if(hasDisplay) {
        vkDisplay.create(vkInstance);
    }

    if(hasDevice) {
        vkDevice.create(vkInstance, vkDisplay);
    }

    if(hasSwapchain) {
        vkSwapchain.create(vkInstance, vkDisplay, vkDevice);
    }

    if(hasPipeline) {
        vkPipeline.create(vkDevice, vkSwapchain);
    }

    hasInstance = true;
}

void VulkanEngine::setDisplay(VulkanDisplay display) {
    if(!hasInstance) {
        std::runtime_error("you can't set the VulkanDisplay without setting a VulkanInstance first");
    }

    if(hasPipeline) {
        vkPipeline.destroyGraphicsPipeline(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain.destroySwapchain(vkDevice);
    }

    if(hasDevice) {
        vkDevice.destroyDevice();
    }

    if(hasDisplay) {
        vkDisplay.destroyDisplay(vkInstance);
    }

    vkDisplay = display;

    vkDisplay.create(vkInstance);

    if(hasDevice) {
        vkDevice.create(vkInstance, vkDisplay);
    }

    if(hasSwapchain) {
        vkSwapchain.create(vkInstance, vkDisplay, vkDevice);
    }

    if(hasPipeline) {
        vkPipeline.create(vkDevice, vkSwapchain);
    }

    hasDisplay = true;
}

void VulkanEngine::setDevice(VulkanDevice device) {
    if(!hasDisplay) {
        std::runtime_error("you can't set the VulkanDevice without setting a VulkanDisplay first");
    }

    if(hasPipeline) {
        vkPipeline.destroyGraphicsPipeline(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain.destroySwapchain(vkDevice);
    }

    if(hasDevice) {
        vkDevice.destroyDevice();
    }

    vkDevice = device;

    vkDevice.create(vkInstance, vkDisplay);

    hasDevice = true;

    if(hasSwapchain) {
        vkSwapchain.create(vkInstance, vkDisplay, vkDevice);
    }

    if(hasPipeline) {
        vkPipeline.create(vkDevice, vkSwapchain);
    }
}

void VulkanEngine::setSwapchain(VulkanSwapchain swapchain) {
    if(!hasDevice) {
        std::runtime_error("you can't set the VulkanSwapchain without setting a VulkanDevice first");
    }

    if(hasPipeline) {
        vkPipeline.destroyGraphicsPipeline(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain.destroySwapchain(vkDevice);
    }

    vkSwapchain = swapchain;

    vkSwapchain.create(vkInstance, vkDisplay, vkDevice);

    if(hasPipeline) {
        vkPipeline.create(vkDevice, vkSwapchain);
    }

    hasSwapchain = true;
}

void VulkanEngine::setGraphicsPipeline(VulkanGraphicsPipeline pipeline) {
    if(!hasSwapchain) {
        std::runtime_error("you can't set the VulkanGraphicsPipeline without setting a VulkanSwapchain first");
    }

    if(hasPipeline) {
        vkPipeline.destroyGraphicsPipeline(vkDevice);
    }

    vkPipeline = pipeline;

    vkPipeline.create(vkDevice, vkSwapchain);

    hasPipeline = true;
}

VulkanDisplay VulkanEngine::getDisplay() {
    return vkDisplay;
}

VulkanInstance VulkanEngine::getInstance() {
    return vkInstance;
}

VulkanDevice VulkanEngine::getDevice() {
    return vkDevice;
}

VulkanSwapchain VulkanEngine::getSwapchain() {
    return vkSwapchain;
}

VulkanGraphicsPipeline VulkanEngine::getGraphicsPipeline() {
    return vkPipeline;
}

void VulkanEngine::engineLoop() {
    if(hasPipeline) { //in order to set pipeline, everything else must also be set so we're good
        glfwPollEvents();
    }else {
        std::runtime_error("VulkanEngine::engineLoop() called before VulkanSwapchain was set");
    }
}