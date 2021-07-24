#include <VulkanRenderSyncObjects.h>

VulkanRenderSyncObjects::VulkanRenderSyncObjects() {

}

void VulkanRenderSyncObjects::create(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain) {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapchain->getInternalImages().size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if(vkCreateSemaphore(device->getInternalLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device->getInternalLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device->getInternalLogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

    hasBeenCreated = true;
}

std::vector<VkSemaphore>& VulkanRenderSyncObjects::getInternalImageAvailableSemaphores() {
    return imageAvailableSemaphores;
}

std::vector<VkSemaphore>& VulkanRenderSyncObjects::getInternalRenderFinishedSemaphores() {
    return renderFinishedSemaphores;
}

std::vector<VkFence>& VulkanRenderSyncObjects::getInternalInFlightFences() {
    return inFlightFences;
}

std::vector<VkFence>& VulkanRenderSyncObjects::getInternalImagesInFlight() {
    return imagesInFlight;
}

int VulkanRenderSyncObjects::getMaxFramesInFlight() {
    return MAX_FRAMES_IN_FLIGHT;
}

bool VulkanRenderSyncObjects::isCreated() {
    return hasBeenCreated;
}

void VulkanRenderSyncObjects::destroySyncObjects(std::shared_ptr<VulkanDevice> device) {
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device->getInternalLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device->getInternalLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device->getInternalLogicalDevice(), inFlightFences[i], nullptr);
    }
}