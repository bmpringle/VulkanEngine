#include "VulkanEngine.h"

VulkanEngine::VulkanEngine() {

}

VulkanEngine::~VulkanEngine() {
    vkSwapchain.destroySwapchain(vkDevice);
    vkDevice.destroyDevice();
    vkDisplay.destroyDisplay(vkInstance);
    vkInstance.destroyInstance();
}

void VulkanEngine::setInstance(VulkanInstance instance) {
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

    hasInstance = true;
}

void VulkanEngine::setDisplay(VulkanDisplay display) {
    if(!hasInstance) {
        std::runtime_error("you can't set the VulkanDisplay without setting a VulkanInstance first");
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

    hasDisplay = true;
}

void VulkanEngine::setDevice(VulkanDevice device) {
    if(!hasDisplay) {
        std::runtime_error("you can't set the VulkanDevice without setting a VulkanDisplay first");
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
}

void VulkanEngine::setSwapchain(VulkanSwapchain swapchain) {
    if(!hasDevice) {
        std::runtime_error("you can't set the VulkanSwapchain without setting a VulkanDevice first");
    }

    if(hasSwapchain) {
        vkSwapchain.destroySwapchain(vkDevice);
    }

    vkSwapchain = swapchain;

    vkSwapchain.create(vkInstance, vkDisplay, vkDevice);

    hasSwapchain = true;
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

void VulkanEngine::engineLoop() {
    if(hasSwapchain) { //in order to set device, everything else must also be set so we're good
        glfwPollEvents();
    }else {
        std::runtime_error("VulkanEngine::engineLoop() called before VulkanSwapchain was set");
    }
}