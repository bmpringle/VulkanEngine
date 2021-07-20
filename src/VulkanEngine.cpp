#include "VulkanEngine.h"

VulkanEngine::VulkanEngine() {

}

VulkanEngine::~VulkanEngine() {
    vkDevice.destroyDevice();
    vkDisplay.destroyDisplay(vkInstance);
    vkInstance.destroyInstance();
}

void VulkanEngine::setInstance(VulkanInstance instance) {
    vkInstance = instance;

    vkInstance.create();

    if(hasDisplay) {
        vkDisplay.create(vkInstance);
    }

    if(hasDevice) {
        vkDevice.create(vkInstance, vkDisplay);
    }

    hasInstance = true;
}

void VulkanEngine::setDisplay(VulkanDisplay display) {
    if(!hasInstance) {
        std::runtime_error("you can't set the VulkanDisplay without setting a VulkanInstance first");
    }

    vkDisplay = display;

    vkDisplay.create(vkInstance);

    if(hasDevice) {
        vkDevice.create(vkInstance, vkDisplay);
    }

    hasDisplay = true;
}

void VulkanEngine::setDevice(VulkanDevice device) {
    if(!hasDisplay) {
        std::runtime_error("you can't set the VulkanDevice without setting a VulkanDisplay first");
    }

    vkDevice = device;

    vkDevice.create(vkInstance, vkDisplay);

    hasDevice = true;
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

void VulkanEngine::engineLoop() {
    if(hasDevice) { //in order to set device, everything else must also be set so we're good
        glfwPollEvents();
    }else {
        std::runtime_error("VulkanEngine::engineLoop() called before VulkanDevice was set");
    }
}