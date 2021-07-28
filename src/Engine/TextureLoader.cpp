#include "Engine/TextureLoader.h"

#include <stdexcept>

std::tuple<int, int, int, stbi_uc*> TextureLoader::getTexturePixels(std::string pathToTexture, int PIXEL_FORMAT_ENUM) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(pathToTexture.data(), &texWidth, &texHeight, &texChannels, PIXEL_FORMAT_ENUM);

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    return std::tuple(texWidth, texHeight, texChannels, pixels);
}