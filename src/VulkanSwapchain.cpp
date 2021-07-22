#include "VulkanSwapchain.h"
#include "SwapChainSupportDetails.h"

#include <algorithm>

VulkanSwapchain::VulkanSwapchain() {

}

void VulkanSwapchain::destroySwapchain(VulkanDevice& device) {
    vkFreeCommandBuffers(device.getInternalLogicalDevice(), device.getInternalCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

    for(auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device.getInternalLogicalDevice(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(device.getInternalLogicalDevice(), renderPass, nullptr);

    for(auto imageView : swapChainImageViews) {
        vkDestroyImageView(device.getInternalLogicalDevice(), imageView, nullptr);
    }

    vkDestroySwapchainKHR(device.getInternalLogicalDevice(), swapchain, nullptr);
}

void VulkanSwapchain::create(VulkanInstance& vkInstance, VulkanDisplay& vkDisplay, VulkanDevice& vkDevice) {
    createSwapchainAndImages(vkInstance, vkDisplay, vkDevice);
    createImageViews(vkInstance, vkDisplay, vkDevice);
    createRenderpass(vkInstance, vkDisplay, vkDevice);
    createFramebuffers(vkDevice);
    createCommandBuffers(vkDevice);
    hasBeenCreated = true;
}

void VulkanSwapchain::createSwapchainAndImages(VulkanInstance& vkInstance, VulkanDisplay& vkDisplay, VulkanDevice& vkDevice) {
    SwapChainSupportDetails swapChainSupport = vkDevice.getDeviceSwapChainSupport(vkDisplay);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, vkDisplay);

    swapChainImageFormat = surfaceFormat.format;

    swapChainExtent = extent;

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1; //get one more than 1 so we never have to wait for the driver

    if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount; //make sure we don't request more than the max possible
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vkDisplay.getInternalSurface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = vkDevice.getDeviceQueueFamilies(vkDisplay);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if(indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; 
        createInfo.pQueueFamilyIndices = nullptr; 
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(vkDevice.getInternalLogicalDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        std::runtime_error("failed to create swapchain!");
    }

    vkGetSwapchainImagesKHR(vkDevice.getInternalLogicalDevice(), swapchain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkDevice.getInternalLogicalDevice(), swapchain, &imageCount, swapChainImages.data());
}

void VulkanSwapchain::createImageViews(VulkanInstance& vkInstance, VulkanDisplay& vkDisplay, VulkanDevice& vkDevice) {
    swapChainImageViews.resize(swapChainImages.size());

    for(size_t i = 0; i < swapChainImageViews.size(); ++i) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if(vkCreateImageView(vkDevice.getInternalLogicalDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("couldn't create VkImageView to correspond to VkImage");
        }
    }
}

void VulkanSwapchain::createRenderpass(VulkanInstance& vkInstance, VulkanDisplay& vkDisplay, VulkanDevice& vkDevice) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;

    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if(vkCreateRenderPass(vkDevice.getInternalLogicalDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

bool VulkanSwapchain::isCreated() {
    return hasBeenCreated;
}

VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for(const auto& availableFormat : availableFormats) {
        if(availableFormat.format == preferredSurfaceFormat && availableFormat.colorSpace == preferredColorSpace) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for(const auto& availablePresentMode : availablePresentModes) {
        if(availablePresentMode == preferredPresentMode) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR; //gauranteed to be available, so fall back to it if it isn't available
}

VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VulkanDisplay& vkDisplay) {
    if(capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }else {
        int width, height;
        glfwGetFramebufferSize(vkDisplay.getInternalWindow(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void VulkanSwapchain::setPreferredPresentMode(VkPresentModeKHR presentMode) {
    preferredPresentMode = presentMode;
}

void VulkanSwapchain::setPreferredSurfaceFormat(VkFormat format) {
    preferredSurfaceFormat = format;
}

void VulkanSwapchain::setPreferredColorSpace(VkColorSpaceKHR colorSpace) {
    preferredColorSpace = colorSpace;
}

VkSwapchainKHR& VulkanSwapchain::getInternalSwapchain() {
    return swapchain;
}

VkFormat& VulkanSwapchain::getInternalSwapchainFormat() {
    return swapChainImageFormat;
}

VkExtent2D& VulkanSwapchain::getInternalExtent2D() {
    return swapChainExtent;
}

std::vector<VkImage>& VulkanSwapchain::getInternalImages() {
    return swapChainImages;
}

std::vector<VkImageView>& VulkanSwapchain::getInternalImageViews() {
    return swapChainImageViews;
}

VkRenderPass& VulkanSwapchain::getInternalRenderPass() {
    return renderPass;
}

void VulkanSwapchain::createFramebuffers(VulkanDevice& device) {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for(size_t i = 0; i < swapChainImageViews.size(); ++i) {
        VkImageView attachments[] = {
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = attachments;
        createInfo.width = swapChainExtent.width;
        createInfo.height = swapChainExtent.height;
        createInfo.layers = 1;

        if(vkCreateFramebuffer(device.getInternalLogicalDevice(), &createInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            std::runtime_error("failed to create a framebuffer corresponding to a vkimageview");
        }
    }
}

std::vector<VkFramebuffer>& VulkanSwapchain::getInternalFramebuffers() {
    return swapChainFramebuffers;
}

void VulkanSwapchain::createCommandBuffers(VulkanDevice& device) {
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = device.getInternalCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if(vkAllocateCommandBuffers(device.getInternalLogicalDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

std::vector<VkCommandBuffer>& VulkanSwapchain::getInternalCommandBuffers() {
    return commandBuffers;
}