#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <tuple>

class TextureLoader {
    //returns width, height, number of channels, pixels
    std::tuple<int, int, int, stbi_uc*> getTexturePixels(std::string pathToTexture, int PIXEL_FORMAT_ENUM);
};

#endif