#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include "VulkanInclude.h"
#include "VulkanDevice.h"

class VulkanSwapchain {

    public:
        VulkanSwapchain();

        void destroySwapchain(VulkanDevice device);

        void create(VulkanInstance vkInstance, VulkanDisplay vkDisplay, VulkanDevice vkDevice);

        bool isCreated();

        void setPreferredPresentMode(VkPresentModeKHR presentMode);

        void setPreferredSurfaceFormat(VkFormat format);

        void setPreferredColorSpace(VkColorSpaceKHR colorSpace);
    private:
        void createSwapchainAndImages(VulkanInstance vkInstance, VulkanDisplay vkDisplay, VulkanDevice vkDevice);

        void createImageViews(VulkanInstance vkInstance, VulkanDisplay vkDisplay, VulkanDevice vkDevice);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VulkanDisplay vkDisplay);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        VkSwapchainKHR swapchain;

        VkFormat swapChainImageFormat;

        VkExtent2D swapChainExtent;

        std::vector<VkImage> swapChainImages;

        std::vector<VkImageView> swapChainImageViews;

        bool hasBeenCreated = false;

        VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

        VkFormat preferredSurfaceFormat = VK_FORMAT_B8G8R8A8_SRGB;

        VkColorSpaceKHR preferredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
};

#endif