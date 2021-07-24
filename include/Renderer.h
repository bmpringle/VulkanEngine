#ifndef RENDERER_H
#define RENDERER_H

#include "Engine/VulkanEngine.h"

#include <memory>

class Renderer {
    public:
        Renderer(std::shared_ptr<VulkanEngine> engine);

        void recordCommandBuffers();
        void renderFrame();
    private:
        std::shared_ptr<VulkanEngine> vkEngine;

        size_t currentFrame = 0;
};

#endif