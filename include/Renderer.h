#ifndef RENDERER_H
#define RENDERER_H

#include "Engine/VulkanEngine.h"

#include <memory>

#include "Vertex.h"

class Renderer {
    public:
        Renderer(std::shared_ptr<VulkanEngine> engine);

        ~Renderer();

        void recordCommandBuffers();

        void renderFrame();

        void setVertexData(std::vector<Vertex>& newVertices);

    private:
        void createVertexBuffer();

        void updateVertexBuffer();

        void destroyVertexBuffer();

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        std::shared_ptr<VulkanEngine> vkEngine;

        std::vector<Vertex> vertices;

        size_t currentFrame = 0;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        uint32_t sizeOfCurrentBuffer = 0;

        void* mappingToVertexBuffer;
};

#endif