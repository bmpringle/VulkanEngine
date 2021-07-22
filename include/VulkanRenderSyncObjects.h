#ifndef VULKANRENDERSYNCOBJECTS_H
#define VULKANRENDERSYNCOBJECTS_H

#include "VulkanSwapchain.h"

class VulkanRenderSyncObjects { 
    public:
        VulkanRenderSyncObjects();

        void destroySyncObjects(VulkanDevice& device);

        void create(VulkanDevice& device, VulkanSwapchain& swapchain);

        std::vector<VkSemaphore>& getInternalImageAvailableSemaphores();

        std::vector<VkSemaphore>& getInternalRenderFinishedSemaphores();

        std::vector<VkFence>& getInternalInFlightFences();

        std::vector<VkFence>& getInternalImagesInFlight();

        int getMaxFramesInFlight();

        bool isCreated();

    private:

        std::vector<VkSemaphore> imageAvailableSemaphores;

        std::vector<VkSemaphore> renderFinishedSemaphores;

        std::vector<VkFence> inFlightFences;

        std::vector<VkFence> imagesInFlight;

        int MAX_FRAMES_IN_FLIGHT = 2;

        bool hasBeenCreated = false;
};

#endif