#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include "VulkanInclude.h"
#include "VulkanDevice.h"
#include "FramebufferAttachment.h"
#include "FramebufferAttachmentInfo.h"
#include <map>
#include "AttachmentDescriptionInfo.h"

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

        VkExtent2D& getInternalExtent2D();

        std::vector<VkFramebuffer>& getInternalFramebuffers();

        std::vector<VkCommandBuffer>& getInternalCommandBuffers();

        VkRenderPass& getInternalRenderPass();

        std::vector<FramebufferAttachment>& getFramebufferAttachment(int index);

        void addFramebufferAttachmentInfo(FramebufferAttachmentInfo info);

        const int getSwapchainAttachmentIndex() const;

        std::vector<FramebufferAttachment>& getSwapchainAttachment();

        int getSwapchainImageCount();

        void addAttachmentDescription(AttachmentDescriptionInfo desc);

        void addSubpassDescription(VkSubpassDescription desc);

        void addSubpassDependency(VkSubpassDependency dependency);

    private:
        std::vector<VkImageView> getAttachmentViewsInOrder(int index);

        void createSwapchainAndImages(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice);

        void createImageViews(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice);

        void createRenderpass(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice);

        void createFramebuffers(std::shared_ptr<VulkanDevice> device);

        void createCommandBuffers(std::shared_ptr<VulkanDevice> device);

        void createUserDefinedAttachments(std::shared_ptr<VulkanDevice> device);
        
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, std::shared_ptr<VulkanDisplay> vkDisplay);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, std::shared_ptr<VulkanDevice> device);

        VkSwapchainKHR swapchain;

        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;

        std::vector<VkCommandBuffer> commandBuffers;

        VkRenderPass renderPass;
        
        bool hasBeenCreated = false;

        VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

        VkFormat preferredSurfaceFormat = VK_FORMAT_B8G8R8A8_SRGB;

        VkColorSpaceKHR preferredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        std::vector<FramebufferAttachmentInfo> userDefinedFramebufferInfo;
        std::vector<std::vector<FramebufferAttachment>> framebufferAttachments;

        int swapchainImageCount = 0;

        std::vector<AttachmentDescriptionInfo> attachmentDescriptionInfos;
        std::vector<VkSubpassDescription> subpassDescriptions;
        std::vector<VkSubpassDependency> subpassDependencies;
};

#endif