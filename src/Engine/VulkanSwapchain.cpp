#include "VulkanSwapchain.h"
#include "SwapChainSupportDetails.h"

#include <algorithm>
#include <array>

#include "VulkanEngine.h"

VulkanSwapchain::VulkanSwapchain() {

}

void VulkanSwapchain::destroySwapchain(std::shared_ptr<VulkanDevice> device) {
    vkFreeCommandBuffers(device->getInternalLogicalDevice(), device->getInternalCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

    for(auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device->getInternalLogicalDevice(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(device->getInternalLogicalDevice(), renderPass, nullptr);

    for(auto imageView : swapChainImageViews) {
        vkDestroyImageView(device->getInternalLogicalDevice(), imageView, nullptr);
    }

    vkDestroySwapchainKHR(device->getInternalLogicalDevice(), swapchain, nullptr);
}

void VulkanSwapchain::create(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice) {
    createSwapchainAndImages(vkInstance, vkDisplay, vkDevice);
    createImageViews(vkInstance, vkDisplay, vkDevice);
    createDepthResources(vkDevice);
    createRenderpass(vkInstance, vkDisplay, vkDevice);
    createFramebuffers(vkDevice);
    createCommandBuffers(vkDevice);
    hasBeenCreated = true;
}

void VulkanSwapchain::createSwapchainAndImages(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice) {
    SwapChainSupportDetails swapChainSupport = vkDevice->getDeviceSwapChainSupport(vkDisplay);

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
    createInfo.surface = vkDisplay->getInternalSurface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = vkDevice->getDeviceQueueFamilies(vkDisplay);
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

    if(vkCreateSwapchainKHR(vkDevice->getInternalLogicalDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        std::runtime_error("failed to create swapchain!");
    }

    vkGetSwapchainImagesKHR(vkDevice->getInternalLogicalDevice(), swapchain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkDevice->getInternalLogicalDevice(), swapchain, &imageCount, swapChainImages.data());
}

void VulkanSwapchain::createImageViews(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice) {
    swapChainImageViews.resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = VulkanEngine::createImageView(swapChainImages[i], swapChainImageFormat, vkDevice);
    }
}

void VulkanSwapchain::createRenderpass(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice) {
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

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, vkDevice
    );
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;

    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if(vkCreateRenderPass(vkDevice->getInternalLogicalDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
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

VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, std::shared_ptr<VulkanDisplay> vkDisplay) {
    if(capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }else {
        int width, height;
        glfwGetFramebufferSize(vkDisplay->getInternalWindow(), &width, &height);

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

void VulkanSwapchain::createFramebuffers(std::shared_ptr<VulkanDevice> device) {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for(size_t i = 0; i < swapChainImageViews.size(); ++i) {
        std::vector<VkImageView> attachments = {
            swapChainImageViews[i],
            depthImageView
        };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachments.data();
        createInfo.width = swapChainExtent.width;
        createInfo.height = swapChainExtent.height;
        createInfo.layers = 1;

        if(vkCreateFramebuffer(device->getInternalLogicalDevice(), &createInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            std::runtime_error("failed to create a framebuffer corresponding to a vkimageview");
        }
    }
}

std::vector<VkFramebuffer>& VulkanSwapchain::getInternalFramebuffers() {
    return swapChainFramebuffers;
}

void VulkanSwapchain::createCommandBuffers(std::shared_ptr<VulkanDevice> device) {
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = device->getInternalCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if(vkAllocateCommandBuffers(device->getInternalLogicalDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

std::vector<VkCommandBuffer>& VulkanSwapchain::getInternalCommandBuffers() {
    return commandBuffers;
}

bool hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat VulkanSwapchain::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, std::shared_ptr<VulkanDevice> device) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device->getInternalPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, std::shared_ptr<VulkanDevice>& device) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device->getInternalPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("couldn't find memory type that matched the requested properties.");
}

VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, std::shared_ptr<VulkanDevice> device) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device->getInternalLogicalDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void VulkanSwapchain::createDepthResources(std::shared_ptr<VulkanDevice> device) {
    VkFormat depthFormat = findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, device);

    VulkanEngine::createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory, device);
    depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, device);
}