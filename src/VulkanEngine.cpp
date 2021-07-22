#include "VulkanEngine.h"

VulkanEngine::VulkanEngine() {

}

VulkanEngine::~VulkanEngine() {
    vkDeviceWaitIdle(vkDevice.getInternalLogicalDevice());

    vkSyncObjects.destroySyncObjects(vkDevice);
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

    if(hasSyncObjects) {
        vkSyncObjects.destroySyncObjects(vkDevice);
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

    if(hasSyncObjects) {
        vkSyncObjects.create(vkDevice, vkSwapchain);
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

    if(hasSyncObjects) {
        vkSyncObjects.destroySyncObjects(vkDevice);
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
    
    if(hasSyncObjects) {
        vkSyncObjects.create(vkDevice, vkSwapchain);
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

    if(hasSyncObjects) {
        vkSyncObjects.destroySyncObjects(vkDevice);
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

    if(hasSyncObjects) {
        vkSyncObjects.create(vkDevice, vkSwapchain);
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

void VulkanEngine::setSyncObjects(VulkanRenderSyncObjects syncObjects) {
    if(!hasSwapchain) {
        std::runtime_error("you can't set the VulkanGraphicsPipeline without setting a VulkanSwapchain first");
    }

    if(hasSyncObjects) {
        vkSyncObjects.destroySyncObjects(vkDevice);
    }

    vkSyncObjects = syncObjects;

    vkSyncObjects.create(vkDevice, vkSwapchain);

    hasSyncObjects = true;
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

VulkanRenderSyncObjects VulkanEngine::getSyncObjects() {
    return vkSyncObjects;
}

void VulkanEngine::engineLoop() {
    if(hasPipeline && hasSyncObjects) { //in order to set pipeline and syncObjects, everything else must also be set so we're good
        recordCommandBuffers();
        drawFrame();
        glfwPollEvents();
    }else {
        std::runtime_error("VulkanEngine::engineLoop() called before VulkanGraphicsPipeline or VulkanRenderSyncObjects were set");
    }
}

void VulkanEngine::recordCommandBuffers() {
    std::vector<VkCommandBuffer>& commandBuffers = vkSwapchain.getInternalCommandBuffers();
    std::vector<VkFramebuffer>& swapChainFramebuffers = vkSwapchain.getInternalFramebuffers();
    VkRenderPass& renderPass = vkSwapchain.getInternalRenderPass();
    VkExtent2D& swapChainExtent = vkSwapchain.getInternalExtent2D();
    VkPipeline& graphicsPipeline = vkPipeline.getInternalGraphicsPipeline();

    std::vector<VkFence>& imagesInFlight = vkSyncObjects.getInternalImagesInFlight();

    for(size_t i = 0; i < commandBuffers.size(); i++) {
        if(imagesInFlight[i] != VK_NULL_HANDLE) {
            vkWaitForFences(vkDevice.getInternalLogicalDevice(), 1, &imagesInFlight[i], VK_TRUE, UINT64_MAX);
        }

        vkResetCommandBuffer(commandBuffers[i], 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;

        VkClearValue clearColor = {
            {
                {
                    0.0f, 0.0f, 0.0f, 1.0f
                }
            }
        };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers[i]);

        if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

void VulkanEngine::drawFrame() {
    VkDevice& device = vkDevice.getInternalLogicalDevice();

    std::vector<VkSemaphore>& imageAvailableSemaphores = vkSyncObjects.getInternalImageAvailableSemaphores();

    std::vector<VkSemaphore>& renderFinishedSemaphores = vkSyncObjects.getInternalImageAvailableSemaphores();

    std::vector<VkFence>& inFlightFences = vkSyncObjects.getInternalInFlightFences();

    std::vector<VkFence>& imagesInFlight = vkSyncObjects.getInternalImagesInFlight();

    std::vector<VkCommandBuffer>& commandBuffers = vkSwapchain.getInternalCommandBuffers();

    VkQueue& graphicsQueue = vkDevice.getInternalGraphicsQueue();
    VkQueue& presentQueue = vkDevice.getInternalPresentQueue();

    VkSwapchainKHR& swapChain = vkSwapchain.getInternalSwapchain();

    int MAX_FRAMES_IN_FLIGHT = vkSyncObjects.getMaxFramesInFlight();

    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
        vkDeviceWaitIdle(device);
        setSwapchain(vkSwapchain);
        return;
    }else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    if(imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional

   result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vkDisplay.getFramebufferResized()) {
        vkDisplay.setFramebufferResized(false);
        vkDeviceWaitIdle(device);
        setSwapchain(vkSwapchain);
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}