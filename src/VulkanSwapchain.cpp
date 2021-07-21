#include "VulkanSwapchain.h"
#include "SwapChainSupportDetails.h"

#include <algorithm>

VulkanSwapchain::VulkanSwapchain() {

}

void VulkanSwapchain::destroySwapchain(VulkanDevice device) {
    for(auto imageView : swapChainImageViews) {
        vkDestroyImageView(device.getInternalLogicalDevice(), imageView, nullptr);
    }

    vkDestroySwapchainKHR(device.getInternalLogicalDevice(), swapchain, nullptr);
}

void VulkanSwapchain::create(VulkanInstance vkInstance, VulkanDisplay vkDisplay, VulkanDevice vkDevice) {
    createSwapchainAndImages(vkInstance, vkDisplay, vkDevice);
    createImageViews(vkInstance, vkDisplay, vkDevice);

    hasBeenCreated = true;
}

void VulkanSwapchain::createSwapchainAndImages(VulkanInstance vkInstance, VulkanDisplay vkDisplay, VulkanDevice vkDevice) {
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

void VulkanSwapchain::createImageViews(VulkanInstance vkInstance, VulkanDisplay vkDisplay, VulkanDevice vkDevice) {
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

VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VulkanDisplay vkDisplay) {
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