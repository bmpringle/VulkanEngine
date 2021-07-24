#include "VulkanEngine.h"

VulkanEngine::VulkanEngine() {

}

VulkanEngine::~VulkanEngine() {
    vkDeviceWaitIdle(vkDevice->getInternalLogicalDevice());

    vkSyncObjects->destroySyncObjects(vkDevice);
    vkPipeline->destroyGraphicsPipeline(vkDevice);
    vkSwapchain->destroySwapchain(vkDevice);
    vkDevice->destroyDevice();
    vkDisplay->destroyDisplay(vkInstance);
    vkInstance->destroyInstance();
}

void VulkanEngine::setInstance(std::shared_ptr<VulkanInstance> instance) {
    if(hasPipeline) {
        vkPipeline->destroyGraphicsPipeline(vkDevice);
    }

    if(hasSyncObjects) {
        vkSyncObjects->destroySyncObjects(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain->destroySwapchain(vkDevice);
    }

    if(hasDevice) {
        vkDevice->destroyDevice();
    }

    if(hasDisplay) {
        vkDisplay->destroyDisplay(vkInstance);
    }

    if(hasInstance) {
        vkInstance->destroyInstance();
    }

    vkInstance = instance;
    vkInstance->create();

    if(hasDisplay) {
        vkDisplay->create(vkInstance);
    }

    if(hasDevice) {
        vkDevice->create(vkInstance, vkDisplay);
    }

    if(hasSwapchain) {
        vkSwapchain->create(vkInstance, vkDisplay, vkDevice);
    }

    if(hasPipeline) {
        vkPipeline->create(vkDevice, vkSwapchain);
    }

    if(hasSyncObjects) {
        vkSyncObjects->create(vkDevice, vkSwapchain);
    }

    hasInstance = true;
}

void VulkanEngine::setDisplay(std::shared_ptr<VulkanDisplay> display) {
    if(!hasInstance) {
        std::runtime_error("you can't set the VulkanDisplay without setting a VulkanInstance first");
    }

    if(hasPipeline) {
        vkPipeline->destroyGraphicsPipeline(vkDevice);
    }

    if(hasSyncObjects) {
        vkSyncObjects->destroySyncObjects(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain->destroySwapchain(vkDevice);
    }

    if(hasDevice) {
        vkDevice->destroyDevice();
    }

    if(hasDisplay) {
        vkDisplay->destroyDisplay(vkInstance);
    }

    vkDisplay = display;

    vkDisplay->create(vkInstance);

    if(hasDevice) {
        vkDevice->create(vkInstance, vkDisplay);
    }

    if(hasSwapchain) {
        vkSwapchain->create(vkInstance, vkDisplay, vkDevice);
    }

    if(hasPipeline) {
        vkPipeline->create(vkDevice, vkSwapchain);
    }
    
    if(hasSyncObjects) {
        vkSyncObjects->create(vkDevice, vkSwapchain);
    }

    hasDisplay = true;
}

void VulkanEngine::setDevice(std::shared_ptr<VulkanDevice> device) {
    if(!hasDisplay) {
        std::runtime_error("you can't set the VulkanDevice without setting a VulkanDisplay first");
    }

    if(hasPipeline) {
        vkPipeline->destroyGraphicsPipeline(vkDevice);
    }

    if(hasSyncObjects) {
        vkSyncObjects->destroySyncObjects(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain->destroySwapchain(vkDevice);
    }

    if(hasDevice) {
        vkDevice->destroyDevice();
    }

    vkDevice = device;

    vkDevice->create(vkInstance, vkDisplay);

    hasDevice = true;

    if(hasSwapchain) {
        vkSwapchain->create(vkInstance, vkDisplay, vkDevice);
    }

    if(hasPipeline) {
        vkPipeline->create(vkDevice, vkSwapchain);
    }

    if(hasSyncObjects) {
        vkSyncObjects->create(vkDevice, vkSwapchain);
    }
}

void VulkanEngine::setSwapchain(std::shared_ptr<VulkanSwapchain> swapchain) {
    if(!hasDevice) {
        std::runtime_error("you can't set the VulkanSwapchain without setting a VulkanDevice first");
    }

    if(hasPipeline) {
        vkPipeline->destroyGraphicsPipeline(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain->destroySwapchain(vkDevice);
    }

    vkSwapchain = swapchain;

    vkSwapchain->create(vkInstance, vkDisplay, vkDevice);

    if(hasPipeline) {
        vkPipeline->create(vkDevice, vkSwapchain);
    }

    hasSwapchain = true;
}

void VulkanEngine::setGraphicsPipeline(std::shared_ptr<VulkanGraphicsPipeline> pipeline) {
    if(!hasSwapchain) {
        std::runtime_error("you can't set the VulkanGraphicsPipeline without setting a VulkanSwapchain first");
    }

    if(hasPipeline) {
        vkPipeline->destroyGraphicsPipeline(vkDevice);
    }

    vkPipeline = pipeline;

    vkPipeline->create(vkDevice, vkSwapchain);

    hasPipeline = true;
}

void VulkanEngine::setSyncObjects(std::shared_ptr<VulkanRenderSyncObjects> syncObjects) {
    if(!hasSwapchain) {
        std::runtime_error("you can't set the VulkanGraphicsPipeline without setting a VulkanSwapchain first");
    }

    if(hasSyncObjects) {
        vkSyncObjects->destroySyncObjects(vkDevice);
    }

    vkSyncObjects = syncObjects;

    vkSyncObjects->create(vkDevice, vkSwapchain);

    hasSyncObjects = true;
}

std::shared_ptr<VulkanDisplay> VulkanEngine::getDisplay() {
    return vkDisplay;
}

std::shared_ptr<VulkanInstance> VulkanEngine::getInstance() {
    return vkInstance;
}

std::shared_ptr<VulkanDevice> VulkanEngine::getDevice() {
    return vkDevice;
}

std::shared_ptr<VulkanSwapchain> VulkanEngine::getSwapchain() {
    return vkSwapchain;
}

std::shared_ptr<VulkanGraphicsPipeline> VulkanEngine::getGraphicsPipeline() {
    return vkPipeline;
}

std::shared_ptr<VulkanRenderSyncObjects> VulkanEngine::getSyncObjects() {
    return vkSyncObjects;
}

void VulkanEngine::recreateSwapchain() {
    vkDeviceWaitIdle(vkDevice->getInternalLogicalDevice());
    setSwapchain(vkSwapchain);
}