#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <string>

#include <tuple>

#include "stb/stb_image.h"


class TextureLoader {
    public:
        //returns width, height, number of channels, pixels
        std::tuple<int, int, int, stbi_uc*> getTexturePixels(std::string pathToTexture, int PIXEL_FORMAT_ENUM);
};

#endif