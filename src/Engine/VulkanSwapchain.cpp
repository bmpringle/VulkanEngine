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

    for(auto colorAttachment1 : colorAttachments1) {
        vkDestroyImageView(device->getInternalLogicalDevice(), colorAttachment1.imageView, nullptr);

        vkDestroyImage(device->getInternalLogicalDevice(), colorAttachment1.image, nullptr);

        vkFreeMemory(device->getInternalLogicalDevice(), colorAttachment1.memory, nullptr);
    }

    for(auto colorAttachment2 : colorAttachments2) {
        vkDestroyImageView(device->getInternalLogicalDevice(), colorAttachment2.imageView, nullptr);

        vkDestroyImage(device->getInternalLogicalDevice(), colorAttachment2.image, nullptr);

        vkFreeMemory(device->getInternalLogicalDevice(), colorAttachment2.memory, nullptr);
    }

    vkDestroyImageView(device->getInternalLogicalDevice(), depthAttachment.imageView, nullptr);

    vkDestroyImage(device->getInternalLogicalDevice(), depthAttachment.image, nullptr);

    vkFreeMemory(device->getInternalLogicalDevice(), depthAttachment.memory, nullptr);

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
        throw std::runtime_error("failed to create swapchain!");
    }

    vkGetSwapchainImagesKHR(vkDevice->getInternalLogicalDevice(), swapchain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    colorAttachments1.resize(imageCount);
    colorAttachments2.resize(imageCount);
    vkGetSwapchainImagesKHR(vkDevice->getInternalLogicalDevice(), swapchain, &imageCount, swapChainImages.data());

    for (uint32_t i = 0; i < colorAttachments1.size(); i++) {
        colorAttachments1[i].format = findSupportedFormat(
        {VK_FORMAT_R16G16B16A16_SFLOAT},
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, vkDevice);

        VulkanEngine::createImage(swapChainExtent.width, swapChainExtent.height, 1, colorAttachments1[i].format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorAttachments1[i].image, colorAttachments1[i].memory, vkDevice);

        colorAttachments2[i].format = findSupportedFormat(
        {VK_FORMAT_R16_SFLOAT},
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, vkDevice);

        VulkanEngine::createImage(swapChainExtent.width, swapChainExtent.height, 1, colorAttachments2[i].format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorAttachments2[i].image, colorAttachments2[i].memory, vkDevice);
    }
}

void VulkanSwapchain::createImageViews(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice) {
    swapChainImageViews.resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = VulkanEngine::createImageView(swapChainImages[i], swapChainImageFormat, vkDevice, VK_IMAGE_VIEW_TYPE_2D, 1);
        colorAttachments1[i].imageView = VulkanEngine::createImageView(colorAttachments1[i].image, colorAttachments1[i].format, vkDevice, VK_IMAGE_VIEW_TYPE_2D, 1);
        colorAttachments2[i].imageView = VulkanEngine::createImageView(colorAttachments2[i].image, colorAttachments2[i].format, vkDevice, VK_IMAGE_VIEW_TYPE_2D, 1);
    }
}

void VulkanSwapchain::createRenderpass(std::shared_ptr<VulkanInstance> vkInstance, std::shared_ptr<VulkanDisplay> vkDisplay, std::shared_ptr<VulkanDevice> vkDevice) {

    //swapchain attachment description/reference

    VkAttachmentDescription swapchainAttachmentDescription{};
    swapchainAttachmentDescription.format = swapChainImageFormat;
    swapchainAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

    swapchainAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    swapchainAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    swapchainAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    swapchainAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    swapchainAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    swapchainAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference swapchainAttachmentRef{};
    swapchainAttachmentRef.attachment = 0;
    swapchainAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //depth attachment description/reference

    VkAttachmentDescription depthAttachmentDescription{};
    depthAttachmentDescription.format = depthAttachment.format;
    depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    //color attachment 1 description/reference
    
    VkAttachmentDescription colorAttachment1Description{};
    colorAttachment1Description.format = colorAttachments1[0].format;
    colorAttachment1Description.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment1Description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment1Description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment1Description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment1Description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment1Description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment1Description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachment1Ref{};
    colorAttachment1Ref.attachment = 2;
    colorAttachment1Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachment1InputRef{};
    colorAttachment1InputRef.attachment = 2;
    colorAttachment1InputRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    //color attachment 2 description/reference
    
    VkAttachmentDescription colorAttachment2Description{};
    colorAttachment2Description.format = colorAttachments2[0].format;
    colorAttachment2Description.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment2Description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment2Description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment2Description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment2Description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment2Description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment2Description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachment2Ref{};
    colorAttachment2Ref.attachment = 3;
    colorAttachment2Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachment2InputRef{};
    colorAttachment2InputRef.attachment = 3;
    colorAttachment2InputRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    //subpass descriptions
    VkSubpassDescription firstSubpass {};
    firstSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    firstSubpass.colorAttachmentCount = 1;
    firstSubpass.pColorAttachments = &swapchainAttachmentRef;
    firstSubpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::vector<VkAttachmentReference> secondSubpassAttachments = {colorAttachment1Ref, colorAttachment2Ref};
    VkSubpassDescription secondSubpass {};
    secondSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    secondSubpass.colorAttachmentCount = secondSubpassAttachments.size();
    secondSubpass.pColorAttachments = secondSubpassAttachments.data();
    secondSubpass.pDepthStencilAttachment = &depthAttachmentRef;
    
    std::vector<VkAttachmentReference> thirdSubpassAttachments = {colorAttachment1InputRef, colorAttachment2InputRef};
    VkSubpassDescription thirdSubpass {};
    thirdSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    thirdSubpass.inputAttachmentCount = thirdSubpassAttachments.size();
    thirdSubpass.pInputAttachments = thirdSubpassAttachments.data();
    thirdSubpass.colorAttachmentCount = 1;
    thirdSubpass.pColorAttachments = &swapchainAttachmentRef;
    thirdSubpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDescription fourthSubpass = firstSubpass;

    //subpass dependencies
    VkSubpassDependency subpassDependencies[4] {};

    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].dstSubpass = 0;

    subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    subpassDependencies[0].srcAccessMask = 0;
    subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].dstSubpass = 1;

    subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    subpassDependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    subpassDependencies[2].srcSubpass = 1;
    subpassDependencies[2].dstSubpass = 2;

    subpassDependencies[2].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[2].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    subpassDependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[2].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    subpassDependencies[3].srcSubpass = 2;
    subpassDependencies[3].dstSubpass = 3;

    subpassDependencies[3].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependencies[3].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    subpassDependencies[3].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[3].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    /*subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].dstSubpass = 0;

    subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependencies[0].srcAccessMask = 0;

    subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].dstSubpass = 1;

    subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    subpassDependencies[2].srcSubpass = 1;
    subpassDependencies[2].dstSubpass = 2;

    subpassDependencies[2].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    subpassDependencies[2].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependencies[2].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;*/

    //render pass info
    std::array<VkAttachmentDescription, 4> attachments = {swapchainAttachmentDescription, depthAttachmentDescription, colorAttachment1Description, colorAttachment2Description};
    std::array<VkSubpassDescription, 4> subpassDescriptions = {firstSubpass, secondSubpass, thirdSubpass, fourthSubpass};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = subpassDescriptions.size();
    renderPassInfo.pSubpasses = subpassDescriptions.data();
    renderPassInfo.dependencyCount = 4;
    renderPassInfo.pDependencies = subpassDependencies;

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
            depthAttachment.imageView,
            colorAttachments1[i].imageView,
            colorAttachments2[i].imageView
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
            throw std::runtime_error("failed to create a framebuffer corresponding to a vkimageview");
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
    depthAttachment.format = findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, device);

    VulkanEngine::createImage(swapChainExtent.width, swapChainExtent.height, 1, depthAttachment.format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthAttachment.image, depthAttachment.memory, device);
    depthAttachment.imageView = createImageView(depthAttachment.image, depthAttachment.format, VK_IMAGE_ASPECT_DEPTH_BIT, device);
}

FramebufferAttachment& VulkanSwapchain::getDepthFramebufferAttachment() {
    return depthAttachment;
}

std::vector<FramebufferAttachment>& VulkanSwapchain::getColorFramebufferAttachments1() {
    return colorAttachments1;
}

std::vector<FramebufferAttachment>& VulkanSwapchain::getColorFramebufferAttachments2() {
    return colorAttachments2;
}