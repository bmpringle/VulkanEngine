#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <string>

#include <tuple>

#include <vector>

#include "stb/stb_image.h"

#include <map>

#include "Engine/VulkanDevice.h"

#include "StringToText/StringToText.h"

class TextureLoader {
    public:
        TextureLoader();

        void create(std::shared_ptr<VulkanDevice> device);
        
        void destroyTextureLoader(std::shared_ptr<VulkanDevice> device);

        //returns width, height, number of channels, pixels
        std::tuple<int, int, int, stbi_uc*> getTexturePixels(std::string pathToTexture, int PIXEL_FORMAT_ENUM);

        VkImageView getImageView(std::string texturePath);

        VkImageView getTextureArrayImageView(std::string arrayID);

        VkSampler getTextureSampler();

        void loadTextureArray(std::shared_ptr<VulkanDevice> device, std::vector<std::string> texturePaths, std::string arrayName);

        void loadTexture(std::shared_ptr<VulkanDevice> device, std::string textureID, std::string texturePath);

        void loadTextToTexture(std::shared_ptr<VulkanDevice> device, std::string textureID, std::string text);

        void copyBufferToImageInLayers(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, std::shared_ptr<VulkanDevice> device, int numberOfLayers);

        std::pair<unsigned int, unsigned int> getTextureDimensions(std::string id);

        std::pair<unsigned int, unsigned int> getTextureArrayDimensions(std::string id);

    private:
        void createTextureImage(std::shared_ptr<VulkanDevice> device, std::string textureID, std::string texturePath);

        void createTextureImageView(std::shared_ptr<VulkanDevice> device, std::string textureID, VkFormat format);

        void createTextureSampler(std::shared_ptr<VulkanDevice> device);

        std::map<std::string, VkImage> texturePathToImage = std::map<std::string, VkImage>();

        std::map<std::string, VkDeviceMemory> texturePathToDeviceMemory = std::map<std::string, VkDeviceMemory>();

        std::map<std::string, VkImageView> texturePathToImageView = std::map<std::string, VkImageView>();

        std::map<std::string, std::pair<unsigned int, unsigned int>> texturePathToImageDimensions = std::map<std::string, std::pair<unsigned int, unsigned int>>();

        VkSampler textureSampler;

        std::map<std::string, VkImage> textureArrayIDToImage = std::map<std::string, VkImage>();

        std::map<std::string, VkDeviceMemory> textureArrayIDToDeviceMemory = std::map<std::string, VkDeviceMemory>();

        std::map<std::string, VkImageView> textureArrayIDToImageView = std::map<std::string, VkImageView>();

        std::map<std::string, std::pair<unsigned int, unsigned int>> textureArrayIDToImageDimensions = std::map<std::string, std::pair<unsigned int, unsigned int>>();


        StringToTextConverter unitypeConverter = StringToTextConverter("assets/unifont-13.0.06.ttf");
};

#endif