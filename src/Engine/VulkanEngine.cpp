#include "VulkanEngine.h"

VulkanEngine::VulkanEngine() : textureLoader(std::make_shared<TextureLoader>()) {

}

VulkanEngine::~VulkanEngine() {
    vkDeviceWaitIdle(vkDevice->getInternalLogicalDevice());

    vkSyncObjects->destroySyncObjects(vkDevice);

    for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
        vkPipeline->destroyGraphicsPipeline(vkDevice);
    }
    
    vkSwapchain->destroySwapchain(vkDevice);
    textureLoader->destroyTextureLoader(vkDevice);
    vkDevice->destroyDevice();
    
    vkDisplay->destroyDisplay(vkInstance);
    vkInstance->destroyInstance();
}

void VulkanEngine::setInstance(std::shared_ptr<VulkanInstance> instance) {
    if(hasPipeline) {
        for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
            vkPipeline->destroyGraphicsPipeline(vkDevice);
        }
    }

    if(hasSyncObjects) {
        vkSyncObjects->destroySyncObjects(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain->destroySwapchain(vkDevice);
    }

    if(hasTextureLoader) {
        textureLoader->destroyTextureLoader(vkDevice);
    }

    if(hasDevice) {
        vkDevice->destroyDevice();
    }

    if(hasDisplay) {
        vkDisplay->destroyDisplay(vkInstance);
    }

    if(hasInstance) {
        vkInstance->destroyInstance();
    }


    vkInstance = instance;
    vkInstance->create();

    if(hasDisplay) {
        vkDisplay->create(vkInstance);
    }

    if(hasDevice) {
        vkDevice->create(vkInstance, vkDisplay);
    }

    if(hasTextureLoader) {
        textureLoader->create(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain->create(vkInstance, vkDisplay, vkDevice);
    }

    if(hasPipeline) {
        for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
            vkPipeline->create(vkDevice, vkSwapchain);
        }
    }

    if(hasSyncObjects) {
        vkSyncObjects->create(vkDevice, vkSwapchain);
    }

    hasInstance = true;
}

void VulkanEngine::setDisplay(std::shared_ptr<VulkanDisplay> display) {
    if(!hasInstance) {
        throw std::runtime_error("you can't set the VulkanDisplay without setting a VulkanInstance first");
    }

    if(hasPipeline) {
        for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
            vkPipeline->destroyGraphicsPipeline(vkDevice);
        }
    }

    if(hasSyncObjects) {
        vkSyncObjects->destroySyncObjects(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain->destroySwapchain(vkDevice);
    }

    if(hasTextureLoader) {
        textureLoader->destroyTextureLoader(vkDevice);
    }

    if(hasDevice) {
        vkDevice->destroyDevice();
    }

    if(hasDisplay) {
        vkDisplay->destroyDisplay(vkInstance);
    }

    vkDisplay = display;

    vkDisplay->create(vkInstance);

    if(hasDevice) {
        vkDevice->create(vkInstance, vkDisplay);
    }

    if(hasTextureLoader) {
        textureLoader->create(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain->create(vkInstance, vkDisplay, vkDevice);
    }

    if(hasPipeline) {
        for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
            vkPipeline->create(vkDevice, vkSwapchain);
        }
    }
    
    if(hasSyncObjects) {
        vkSyncObjects->create(vkDevice, vkSwapchain);
    }

    hasDisplay = true;
}

void VulkanEngine::setDevice(std::shared_ptr<VulkanDevice> device) {
    if(!hasDisplay) {
        throw std::runtime_error("you can't set the VulkanDevice without setting a VulkanDisplay first");
    }

    if(hasPipeline) {
        for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
            vkPipeline->destroyGraphicsPipeline(vkDevice);
        }
    }

    if(hasSyncObjects) {
        vkSyncObjects->destroySyncObjects(vkDevice);
    }

    if(hasSwapchain) {
        vkSwapchain->destroySwapchain(vkDevice);
    }

    if(hasTextureLoader) {
        textureLoader->destroyTextureLoader(vkDevice);
    }

    if(hasDevice) {
        vkDevice->destroyDevice();
    }

    vkDevice = device;

    vkDevice->create(vkInstance, vkDisplay);

    hasDevice = true;

    textureLoader->create(vkDevice);
    hasTextureLoader = true;

    if(hasSwapchain) {
        vkSwapchain->create(vkInstance, vkDisplay, vkDevice);
    }

    if(hasPipeline) {
        for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
            vkPipeline->create(vkDevice, vkSwapchain);
        }
    }

    if(hasSyncObjects) {
        vkSyncObjects->create(vkDevice, vkSwapchain);
    }
}

void VulkanEngine::setSwapchain(std::shared_ptr<VulkanSwapchain> swapchain) {
    if(!hasDevice) {
        throw std::runtime_error("you can't set the VulkanSwapchain without setting a VulkanDevice first");
    }

    if(hasPipeline) {
        for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
            vkPipeline->destroyGraphicsPipeline(vkDevice);
        }
    }

    if(hasSwapchain) {
        vkSwapchain->destroySwapchain(vkDevice);
    }

    vkSwapchain = swapchain;

    vkSwapchain->create(vkInstance, vkDisplay, vkDevice);

    if(hasPipeline) {
        for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
            vkPipeline->create(vkDevice, vkSwapchain);
        }
    }

    hasSwapchain = true;
}

void VulkanEngine::setGraphicsPipeline(std::shared_ptr<VulkanGraphicsPipeline> pipeline, int index) {
    if(!hasSwapchain) {
        throw std::runtime_error("you can't set the VulkanGraphicsPipeline without setting a VulkanSwapchain first");
    }

    if(hasPipeline) {
        for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
            vkPipeline->destroyGraphicsPipeline(vkDevice);
        }
    }

    if(index < vkPipelines.size()) {
        vkPipelines[index] = pipeline;
    }else if(index == vkPipelines.size()) {
        vkPipelines.push_back(pipeline);
    }else {
        throw std::runtime_error("you can't add a pipeline with an index greater than the current pipeline vector's size!");
    }

    hasPipeline = true;

    if(hasPipeline) {
        for(std::shared_ptr<VulkanGraphicsPipeline> vkPipeline : vkPipelines) {
            vkPipeline->create(vkDevice, vkSwapchain);
        }
    }    
}

void VulkanEngine::setSyncObjects(std::shared_ptr<VulkanRenderSyncObjects> syncObjects) {
    if(!hasSwapchain) {
        throw std::runtime_error("you can't set the VulkanGraphicsPipeline without setting a VulkanSwapchain first");
    }

    if(hasSyncObjects) {
        vkSyncObjects->destroySyncObjects(vkDevice);
    }

    vkSyncObjects = syncObjects;

    vkSyncObjects->create(vkDevice, vkSwapchain);

    hasSyncObjects = true;
}

std::shared_ptr<VulkanDisplay> VulkanEngine::getDisplay() {
    return vkDisplay;
}

std::shared_ptr<VulkanInstance> VulkanEngine::getInstance() {
    return vkInstance;
}

std::shared_ptr<VulkanDevice> VulkanEngine::getDevice() {
    return vkDevice;
}

std::shared_ptr<VulkanSwapchain> VulkanEngine::getSwapchain() {
    return vkSwapchain;
}

std::shared_ptr<VulkanGraphicsPipeline> VulkanEngine::getGraphicsPipeline(int pipelineIndex) {
    return vkPipelines[pipelineIndex];
}

std::shared_ptr<VulkanRenderSyncObjects> VulkanEngine::getSyncObjects() {
    return vkSyncObjects;
}

std::shared_ptr<TextureLoader> VulkanEngine::getTextureLoader() {
    return textureLoader;
}

void VulkanEngine::recreateSwapchain() {
    vkDeviceWaitIdle(vkDevice->getInternalLogicalDevice());
    setSwapchain(vkSwapchain);
}

uint32_t VulkanEngine::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, std::shared_ptr<VulkanDevice>  device) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device->getInternalPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("couldn't find memory type that matched the requested properties.");
}

void VulkanEngine::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, std::shared_ptr<VulkanDevice>  device) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device->getInternalLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device->getInternalLogicalDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanEngine::findMemoryType(memRequirements.memoryTypeBits, properties, device);

    if(vkAllocateMemory(device->getInternalLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(device->getInternalLogicalDevice(), buffer, bufferMemory, 0);
}

void VulkanEngine::createImage(uint32_t width, uint32_t height, uint32_t layers, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, std::shared_ptr<VulkanDevice> device) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = layers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device->getInternalLogicalDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->getInternalLogicalDevice(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, device);

    if (vkAllocateMemory(device->getInternalLogicalDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device->getInternalLogicalDevice(), image, imageMemory, 0);
}

VkCommandBuffer VulkanEngine::beginSingleTimeCommands(std::shared_ptr<VulkanDevice> device) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device->getInternalCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device->getInternalLogicalDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer, std::shared_ptr<VulkanDevice> device) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device->getInternalGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->getInternalGraphicsQueue());

    vkFreeCommandBuffers(device->getInternalLogicalDevice(), device->getInternalCommandPool(), 1, &commandBuffer);
}

void VulkanEngine::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, std::shared_ptr<VulkanDevice> device) {
    VkCommandBuffer commandBuffer = VulkanEngine::beginSingleTimeCommands(device);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    VulkanEngine::endSingleTimeCommands(commandBuffer, device);
}

void VulkanEngine::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, std::shared_ptr<VulkanDevice> device, int layerCount) {
    VkCommandBuffer commandBuffer = VulkanEngine::beginSingleTimeCommands(device);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layerCount;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }else{
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    VulkanEngine::endSingleTimeCommands(commandBuffer, device);
}

void VulkanEngine::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, std::shared_ptr<VulkanDevice> device) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    endSingleTimeCommands(commandBuffer, device);
}

VkImageView VulkanEngine::createImageView(VkImage image, VkFormat format, std::shared_ptr<VulkanDevice> device, VkImageViewType type, int layerCount) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.viewType = type;
    viewInfo.format = format;
    
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = layerCount;
    viewInfo.image = image;

    VkImageView imageView;
    if (vkCreateImageView(device->getInternalLogicalDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}