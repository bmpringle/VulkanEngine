#include "Engine/TextureLoader.h"

#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "Engine/VulkanEngine.h"

#include <functional>

TextureLoader::TextureLoader() {

}

std::tuple<int, int, int, stbi_uc*> TextureLoader::getTexturePixels(std::string pathToTexture, int PIXEL_FORMAT_ENUM) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(pathToTexture.data(), &texWidth, &texHeight, &texChannels, PIXEL_FORMAT_ENUM);

    if (!pixels) {
        throw std::runtime_error("failed to load texture image " + pathToTexture + "!");
    }

    return std::tuple(texWidth, texHeight, texChannels, pixels);
}

void TextureLoader::createTextureImage(std::shared_ptr<VulkanDevice> device, std::string textureID, std::string texturePath, bool* deleteOldTextureBool) {
    std::tuple<int, int, int, stbi_uc*> textureData = getTexturePixels(texturePath, STBI_rgb_alpha);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkDeviceSize imageSize = std::get<0>(textureData) * std::get<1>(textureData) * 4;

    VulkanEngine::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device);

    void* data;
    vkMapMemory(device->getInternalLogicalDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, std::get<3>(textureData), static_cast<size_t>(imageSize));
    vkUnmapMemory(device->getInternalLogicalDevice(), stagingBufferMemory);

    stbi_image_free(std::get<3>(textureData));

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VulkanEngine::createImage(std::get<0>(textureData), std::get<1>(textureData), 1, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, device);

    VulkanEngine::transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, device, 1);
    VulkanEngine::copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(std::get<0>(textureData)), static_cast<uint32_t>(std::get<1>(textureData)), device);

    VulkanEngine::transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, device, 1);

    vkDestroyBuffer(device->getInternalLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device->getInternalLogicalDevice(), stagingBufferMemory, nullptr);

    texturePathToImage[textureID] = textureImage;
    texturePathToDeviceMemory[textureID] = textureImageMemory;

    texturePathToImageDimensions[textureID] = std::make_pair(std::get<0>(textureData), std::get<1>(textureData));
}

void TextureLoader::createTextureImageView(std::shared_ptr<VulkanDevice> device, std::string textureID, VkFormat format) {
    texturePathToImageView[textureID] = VulkanEngine::createImageView(texturePathToImage[textureID], format, device, VK_IMAGE_VIEW_TYPE_2D, 1);
}

void TextureLoader::createTextureSampler(std::shared_ptr<VulkanDevice> device) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(device->getInternalPhysicalDevice(), &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if(vkCreateSampler(device->getInternalLogicalDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

VkImageView TextureLoader::getImageView(std::string texturePath) {
    if(texturePathToImageView.count(texturePath) == 0) {
        throw std::runtime_error("no imageview with id" + texturePath + " found");
    }
    return texturePathToImageView[texturePath];
}

VkSampler TextureLoader::getTextureSampler() {
    return textureSampler;
}

void TextureLoader::create(std::shared_ptr<VulkanDevice> device) {
    funcFreeImage = std::bind(&TextureLoader::vkDeleteImage, this, device, std::placeholders::_1);
    funcFreeImageView = std::bind(&TextureLoader::vkDeleteImageView, this, device, std::placeholders::_1);
    funcFreeDeviceMemory = std::bind(&TextureLoader::vkDeleteDeviceMemory, this, device, std::placeholders::_1);
    imageDeleteThread = std::make_shared<DeleteThread<VkImage>>(funcFreeImage);
    imageViewDeleteThread = std::make_shared<DeleteThread<VkImageView>>(funcFreeImageView);
    deviceMemoryDeleteThread = std::make_shared<DeleteThread<VkDeviceMemory>>(funcFreeDeviceMemory);

    createTextureSampler(device);
}

void TextureLoader::destroyTextureLoader(std::shared_ptr<VulkanDevice> device) {
    vkDestroySampler(device->getInternalLogicalDevice(), textureSampler, nullptr);

    for(std::pair<const std::string, VkImageView> imageViewPair : textureArrayIDToImageView) {
        vkDestroyImageView(device->getInternalLogicalDevice(), imageViewPair.second, nullptr);
    }

    for(std::pair<const std::string, VkImage> imagePair : textureArrayIDToImage) {
        vkDestroyImage(device->getInternalLogicalDevice(), imagePair.second, nullptr);
    }
    
    for(std::pair<const std::string, VkDeviceMemory> imagePair : textureArrayIDToDeviceMemory) {
        vkFreeMemory(device->getInternalLogicalDevice(), imagePair.second, nullptr);
    }

    for(std::pair<const std::string, VkImageView> imageViewPair : texturePathToImageView) {
        vkDestroyImageView(device->getInternalLogicalDevice(), imageViewPair.second, nullptr);
    }

    for(std::pair<const std::string, VkImage> imagePair : texturePathToImage) {
        vkDestroyImage(device->getInternalLogicalDevice(), imagePair.second, nullptr);
    }
    
    for(std::pair<const std::string, VkDeviceMemory> imagePair : texturePathToDeviceMemory) {
        vkFreeMemory(device->getInternalLogicalDevice(), imagePair.second, nullptr);
    }

    imageViewDeleteThread->forceJoin();
    imageDeleteThread->forceJoin();
    deviceMemoryDeleteThread->forceJoin();
}

void TextureLoader::loadTexture(std::shared_ptr<VulkanDevice> device, std::string textureID, std::string texturePath, bool* deleteOldTextureBool) {
    VkImage oldImage = texturePathToImage[textureID];
    VkImageView oldImageView = texturePathToImageView[textureID];
    VkDeviceMemory oldDeviceMemory = texturePathToDeviceMemory[textureID];

    createTextureImage(device, textureID, texturePath, deleteOldTextureBool);
    createTextureImageView(device, textureID, VK_FORMAT_R8G8B8A8_SRGB);

    if(texturePathToImage.count(textureID) > 0) {
        imageViewDeleteThread->addObjectToDelete(oldImageView, deleteOldTextureBool);

        imageDeleteThread->addObjectToDelete(oldImage, deleteOldTextureBool);

        deviceMemoryDeleteThread->addObjectToDelete(oldDeviceMemory, deleteOldTextureBool);
    }
}

void TextureLoader::loadTextureArray(std::shared_ptr<VulkanDevice> device, std::vector<std::string> texturePaths, std::string arrayName, bool* deleteOldTextureBool) {
    VkImage oldImage = textureArrayIDToImage[arrayName];
    VkImageView oldImageView = textureArrayIDToImageView[arrayName];
    VkDeviceMemory oldDeviceMemory = texturePathToDeviceMemory[arrayName];
    
    std::vector<std::tuple<int, int, int, stbi_uc*>> textureData;

    for(std::string& path : texturePaths) {
        textureData.push_back(getTexturePixels(path, STBI_rgb_alpha));
    }

    int width = -1;
    int height = -1;

    for(std::tuple<int, int, int, stbi_uc*>& data : textureData) {
        if(width == -1 || height == -1) {
            width = std::get<0>(data);
            height = std::get<1>(data);
            continue;
        }

        if(std::get<0>(data) != width || std::get<1>(data) != height) {
            throw std::runtime_error("not all textures in loadTextureArray are the same width/height");
        }
    }
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkDeviceSize layerSize = std::get<0>(textureData.at(0)) * std::get<1>(textureData.at(0)) * 4;

    VkDeviceSize imageSize = textureData.size() * layerSize;

    VulkanEngine::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device);

    VkDeviceSize bufferOffset = 0;

    void* buffer;
    for(std::tuple<int, int, int, stbi_uc*>& data : textureData) {
        vkMapMemory(device->getInternalLogicalDevice(), stagingBufferMemory, bufferOffset, layerSize, 0, &buffer);
        memcpy(buffer, std::get<3>(data), static_cast<size_t>(layerSize));
        vkUnmapMemory(device->getInternalLogicalDevice(), stagingBufferMemory);
        bufferOffset = bufferOffset + layerSize;
    }

    for(std::tuple<int, int, int, stbi_uc*>& data : textureData) {
        stbi_image_free(std::get<3>(data));
    }

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VulkanEngine::createImage(std::get<0>(textureData.at(0)), std::get<1>(textureData.at(0)), textureData.size(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, device);

    VulkanEngine::transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, device, texturePaths.size());
    TextureLoader::copyBufferToImageInLayers(stagingBuffer, textureImage, static_cast<uint32_t>(std::get<0>(textureData.at(0))), static_cast<uint32_t>(std::get<1>(textureData.at(0))), device, texturePaths.size());

    VulkanEngine::transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, device, textureData.size());

    textureArrayIDToImage[arrayName] = textureImage;

    textureArrayIDToDeviceMemory[arrayName] = textureImageMemory;

    textureArrayIDToImageDimensions[arrayName] = std::make_pair(std::get<0>(textureData.at(0)), std::get<1>(textureData.at(0)));

    textureArrayIDToImageView[arrayName] = VulkanEngine::createImageView(textureArrayIDToImage[arrayName], VK_FORMAT_R8G8B8A8_SRGB, device, VK_IMAGE_VIEW_TYPE_2D_ARRAY, texturePaths.size());

    vkDestroyBuffer(device->getInternalLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device->getInternalLogicalDevice(), stagingBufferMemory, nullptr);

    if(textureArrayIDToImage.count(arrayName) > 0) {
        imageViewDeleteThread->addObjectToDelete(oldImageView, deleteOldTextureBool);

        imageDeleteThread->addObjectToDelete(oldImage, deleteOldTextureBool);

        deviceMemoryDeleteThread->addObjectToDelete(oldDeviceMemory, deleteOldTextureBool);
    }
}

VkImageView TextureLoader::getTextureArrayImageView(std::string arrayID) {
    return textureArrayIDToImageView[arrayID];
}

void TextureLoader::copyBufferToImageInLayers(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, std::shared_ptr<VulkanDevice> device, int numberOfLayers) {
    VkCommandBuffer commandBuffer = VulkanEngine::beginSingleTimeCommands(device);

    VkDeviceSize bufferOffset = 0;

    std::vector<VkBufferImageCopy> regions;

    for(int layer = 0; layer < numberOfLayers; ++layer) {
        VkBufferImageCopy region{};
        region.bufferOffset = bufferOffset;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = layer;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        regions.push_back(region);

        bufferOffset = bufferOffset + width * height * 4;
    }

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        regions.size(),
        regions.data()
    );

    VulkanEngine::endSingleTimeCommands(commandBuffer, device);
}

void expandBitmapChannels(TextBitmap* bitmap, glm::vec3 textColor) {
    std::vector<unsigned char> expandedBitmap;
    for(unsigned char c : bitmap->bitmap) {
        expandedBitmap.push_back(textColor[0]);
        expandedBitmap.push_back(textColor[1]);
        expandedBitmap.push_back(textColor[2]);
        expandedBitmap.push_back(c);
    }
    bitmap->bitmap = expandedBitmap;
    bitmap->stride = bitmap->stride;
    bitmap->rows = bitmap->rows;
}

void TextureLoader::loadTextToTexture(std::shared_ptr<VulkanDevice> device, std::string textureID, std::string text, glm::vec3 textColor, bool* deleteOldTextureBool) {
    VkImage oldImage = texturePathToImage[textureID];
    VkImageView oldImageView = texturePathToImageView[textureID];
    VkDeviceMemory oldDeviceMemory = texturePathToDeviceMemory[textureID];

    TextBitmap bitmap = unitypeConverter.getTextFromString(text);
    expandBitmapChannels(&bitmap, textColor);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkDeviceSize imageSize = 4 * bitmap.rows * bitmap.stride;

    VulkanEngine::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device);

    void* data;
    vkMapMemory(device->getInternalLogicalDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, bitmap.bitmap.data(), static_cast<size_t>(imageSize));
    vkUnmapMemory(device->getInternalLogicalDevice(), stagingBufferMemory);

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VulkanEngine::createImage(bitmap.stride, bitmap.rows, 1, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, device);

    VulkanEngine::transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, device, 1);
    VulkanEngine::copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(bitmap.stride), static_cast<uint32_t>(bitmap.rows), device);
    
    VulkanEngine::transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, device, 1);

    vkDestroyBuffer(device->getInternalLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device->getInternalLogicalDevice(), stagingBufferMemory, nullptr);

    texturePathToImage[textureID] = textureImage;
    texturePathToDeviceMemory[textureID] = textureImageMemory;

    texturePathToImageDimensions[textureID] = std::make_pair(bitmap.stride, bitmap.rows);

    createTextureImageView(device, textureID, VK_FORMAT_R8G8B8A8_SRGB);

    if(texturePathToImage.count(textureID) > 0) {
        imageViewDeleteThread->addObjectToDelete(oldImageView, deleteOldTextureBool);

        imageDeleteThread->addObjectToDelete(oldImage, deleteOldTextureBool);

        deviceMemoryDeleteThread->addObjectToDelete(oldDeviceMemory, deleteOldTextureBool);
    }   
}

std::pair<unsigned int, unsigned int> TextureLoader::getTextureDimensions(std::string id) {
    if(texturePathToImageDimensions.count(id) == 0) {
        throw std::runtime_error("no texture dimensions with id" + id + " found");
    }

    return texturePathToImageDimensions.at(id);
}

std::pair<unsigned int, unsigned int> TextureLoader::getTextureArrayDimensions(std::string id) {
    if(textureArrayIDToImageDimensions.count(id) == 0) {
        throw std::runtime_error("no texture dimensions with id" + id + " found");
    }

    return textureArrayIDToImageDimensions.at(id);
}

std::mutex* TextureLoader::getImageDeleteThreadAccessMutex() {
    if(!imageDeleteThread->isValidInstance()) {
        abort();
    }else {
        imageDeleteThread->getMutexPointer()->lock();
        imageDeleteThread->getMutexPointer()->unlock();
    }
    return imageDeleteThread->getMutexPointer();
}