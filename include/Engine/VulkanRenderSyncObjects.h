#ifndef VULKANRENDERSYNCOBJECTS_H
#define VULKANRENDERSYNCOBJECTS_H

#include "VulkanSwapchain.h"

class VulkanRenderSyncObjects { 
    public:
        VulkanRenderSyncObjects();

        void destroySyncObjects(std::shared_ptr<VulkanDevice> device);

        void create(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain);

        std::vector<VkSemaphore>& getInternalImageAvailableSemaphores();

        std::vector<VkSemaphore>& getInternalRenderFinishedSemaphores();

        std::vector<VkFence>& getInternalInFlightFences();

        std::vector<VkFence>& getInternalImagesInFlight();

        constexpr static const int getMaxFramesInFlight() {
            return MAX_FRAMES_IN_FLIGHT;
        };

        bool isCreated();

    private:

        std::vector<VkSemaphore> imageAvailableSemaphores;

        std::vector<VkSemaphore> renderFinishedSemaphores;

        std::vector<VkFence> inFlightFences;

        std::vector<VkFence> imagesInFlight;

        static const int MAX_FRAMES_IN_FLIGHT = 2;

        bool hasBeenCreated = false;
};

#endif