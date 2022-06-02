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

    bool SWAPCHAIN_ATTACHMENT = true;

    for(auto attachmentVector : framebufferAttachments) {
        for(auto attachment : attachmentVector) {
            vkDestroyImageView(device->getInternalLogicalDevice(), attachment.imageView, nullptr);

            if(!SWAPCHAIN_ATTACHMENT) { //don't destroy the first attachment here b/c it's the swapchain, and should be destroyed automatically
                vkDestroyImage(device->getInternalLogicalDevice(), attachment.image, nullptr);

                vkFreeMemory(device->getInternalLogicalDevice(), attachment.memory, nullptr);
            }
        }
        SWAPCHAIN_ATTACHMENT = false;
    }

    vkDestroySwapchainKHR(device->getInternalLogicalDevice(), swapchain, nullptr);
}

void VulkanSwapchain::create(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice) {
    createSwapchainAndImages(vkInstance, vkDisplay, vkDevice);
    createImageViews(vkInstance, vkDisplay, vkDevice);
    createUserDefinedAttachments(vkDevice);
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
        throw std::runtime_error("failed to create swapchain!");
    }

    vkGetSwapchainImagesKHR(vkDevice->getInternalLogicalDevice(), swapchain, &imageCount, nullptr);
    framebufferAttachments.resize(1);
    framebufferAttachments[0].resize(imageCount);
    swapchainImageCount = imageCount;

    std::vector<VkImage> swapImages;
    swapImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkDevice->getInternalLogicalDevice(), swapchain, &imageCount, swapImages.data());

    for(int i = 0; i < imageCount; ++i) {
        framebufferAttachments[0][i].image = swapImages[i];
        framebufferAttachments[0][i].format = surfaceFormat.format;
    }
}

void VulkanSwapchain::createImageViews(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice) {
    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        framebufferAttachments[0][i].imageView = VulkanEngine::createImageView(framebufferAttachments[0][i].image, framebufferAttachments[0][i].format, vkDevice, VK_IMAGE_VIEW_TYPE_2D, 1);
    }
}

void VulkanSwapchain::createRenderpass(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice) {
    std::vector<VkAttachmentDescription> attachmentDescriptions;

    for(auto info : attachmentDescriptionInfos) {
        VkAttachmentDescription desc;
        desc.format = getFramebufferAttachment(info.attachmentIndex)[0].format;
        desc.samples = info.samples;

        desc.loadOp = info.loadOp;
        desc.storeOp = info.storeOp;

        desc.stencilLoadOp = info.stencilLoadOp;
        desc.stencilStoreOp = info.stencilStoreOp;

        desc.initialLayout = info.initialLayout;
        desc.finalLayout = info.finalLayout;

        attachmentDescriptions.push_back(desc);
        desc.flags = 0;
    }
    
    //render pass info
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    renderPassInfo.pAttachments = attachmentDescriptions.data();
    renderPassInfo.subpassCount = subpassDescriptions.size();
    renderPassInfo.pSubpasses = subpassDescriptions.data();
    renderPassInfo.dependencyCount = subpassDependencies.size();
    renderPassInfo.pDependencies = subpassDependencies.data();

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

std::vector<FramebufferAttachment>& VulkanSwapchain::getSwapchainAttachment() {
    return framebufferAttachments[0];
}

VkExtent2D& VulkanSwapchain::getInternalExtent2D() {
    return swapChainExtent;
}

VkRenderPass& VulkanSwapchain::getInternalRenderPass() {
    return renderPass;
}

void VulkanSwapchain::createFramebuffers(std::shared_ptr<VulkanDevice> device) {
    swapChainFramebuffers.resize(swapchainImageCount);

    for(size_t i = 0; i < swapchainImageCount; ++i) {
        std::vector<VkImageView> attachments = getAttachmentViewsInOrder(i);

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachments.data();
        createInfo.width = swapChainExtent.width;
        createInfo.height = swapChainExtent.height;
        createInfo.layers = 1;

        if(vkCreateFramebuffer(device->getInternalLogicalDevice(), &createInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create a framebuffer corresponding to a vkimageview");
        }
    }
}

std::vector<VkFramebuffer>& VulkanSwapchain::getInternalFramebuffers() {
    return swapChainFramebuffers;
}

void VulkanSwapchain::createCommandBuffers(std::shared_ptr<VulkanDevice> device) {
    commandBuffers.resize(swapchainImageCount);

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

void VulkanSwapchain::createUserDefinedAttachments(std::shared_ptr<VulkanDevice> device) {
    for(FramebufferAttachmentInfo& info : userDefinedFramebufferInfo) {
        std::vector<FramebufferAttachment> attachmentVector;
        int sizeOfVector = (info.count != SWAPCHAIN_COUNT) ? 1 : swapchainImageCount;
        for(int c = 0; c < sizeOfVector; ++c) {
            FramebufferAttachment attachment;

            attachment.format = findSupportedFormat(info.formatCandidates, info.imageTiling, info.formatFeatures, device);
            VulkanEngine::createImage(swapChainExtent.width, swapChainExtent.height, 1, attachment.format, info.imageTiling, info.usageBits, info.memoryPropertyBits, attachment.image, attachment.memory, device);
            attachment.imageView = createImageView(attachment.image, attachment.format, info.imageAspectBits, device);

            attachmentVector.push_back(attachment);
        }

        if(framebufferAttachments.size() <= info.index) {
            framebufferAttachments.resize(info.index + 1);
            framebufferAttachments.at(info.index) = attachmentVector;
        }else {
            throw std::runtime_error("You can't use the same name for more than one framebuffer attachment!"); 
            //I don't allow mutability b/c I'm really not sure if that would work or cause a ton of bugs, and given the usage of this function it just isn't necessary
        }
    }
}

std::vector<FramebufferAttachment>& VulkanSwapchain::getFramebufferAttachment(int index) {
    return framebufferAttachments.at(index);
}

void VulkanSwapchain::addFramebufferAttachmentInfo(FramebufferAttachmentInfo info) {
    userDefinedFramebufferInfo.push_back(info);
}

const int VulkanSwapchain::getSwapchainAttachmentIndex() const {
    return 0;
}

std::vector<VkImageView> VulkanSwapchain::getAttachmentViewsInOrder(int index) {
    std::vector<VkImageView> views;

    for(auto attachment : framebufferAttachments) {
        if(attachment.size() == 1) {
            views.push_back(attachment.at(0).imageView);
        }else {
            views.push_back(attachment.at(index).imageView);
        }
    }

    return views;
}

int VulkanSwapchain::getSwapchainImageCount() {
    return swapchainImageCount;
}

void VulkanSwapchain::addAttachmentDescription(AttachmentDescriptionInfo desc) {
    attachmentDescriptionInfos.push_back(desc);
}

void VulkanSwapchain::addSubpassDescription(VkSubpassDescription desc) {
    subpassDescriptions.push_back(desc);
}

void VulkanSwapchain::addSubpassDependency(VkSubpassDependency dependency) {
    subpassDependencies.push_back(dependency);
}