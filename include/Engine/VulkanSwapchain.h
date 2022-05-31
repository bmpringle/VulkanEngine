#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include "VulkanInclude.h"
#include "VulkanDevice.h"
#include "FramebufferAttachment.h"

class VulkanSwapchain {

    public:
        VulkanSwapchain();

        void destroySwapchain(std::shared_ptr<VulkanDevice> device);

        void create(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice);

        bool isCreated();

        void setPreferredPresentMode(VkPresentModeKHR presentMode);

        void setPreferredSurfaceFormat(VkFormat format);

        void setPreferredColorSpace(VkColorSpaceKHR colorSpace);

        VkSwapchainKHR& getInternalSwapchain();

        VkFormat& getInternalSwapchainFormat();

        VkExtent2D& getInternalExtent2D();

        std::vector<VkImage>& getInternalImages();

        std::vector<VkImageView>& getInternalImageViews();

        std::vector<VkFramebuffer>& getInternalFramebuffers();

        std::vector<VkCommandBuffer>& getInternalCommandBuffers();

        VkRenderPass& getInternalRenderPass();

        FramebufferAttachment& getDepthFramebufferAttachment();

        std::vector<FramebufferAttachment>& getColorFramebufferAttachments1();

        std::vector<FramebufferAttachment>& getColorFramebufferAttachments2();

    private:
        void createSwapchainAndImages(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice);

        void createImageViews(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice);

        void createRenderpass(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice);

        void createFramebuffers(std::shared_ptr<VulkanDevice> device);

        void createCommandBuffers(std::shared_ptr<VulkanDevice> device);

        void createDepthResources(std::shared_ptr<VulkanDevice> device);
        
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, std::shared_ptr<VulkanDisplay> vkDisplay);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, std::shared_ptr<VulkanDevice> device);

        VkSwapchainKHR swapchain;

        VkFormat swapChainImageFormat;

        VkExtent2D swapChainExtent;

        std::vector<VkImage> swapChainImages;

        std::vector<VkImageView> swapChainImageViews;

        std::vector<VkFramebuffer> swapChainFramebuffers;

        std::vector<VkCommandBuffer> commandBuffers;

        std::vector<FramebufferAttachment> colorAttachments1;
        std::vector<FramebufferAttachment> colorAttachments2;

        FramebufferAttachment depthAttachment;

        VkRenderPass renderPass;
        
        bool hasBeenCreated = false;

        VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

        VkFormat preferredSurfaceFormat = VK_FORMAT_B8G8R8A8_SRGB;

        VkColorSpaceKHR preferredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
};

#endif