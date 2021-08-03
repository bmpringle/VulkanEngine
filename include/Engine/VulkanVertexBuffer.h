#ifndef VULKANVERTEXBUFFER_H
#define VULKANVERTEXBUFFER_H

#include "Vertex.h"

#include "Engine/VulkanEngine.h"

class VulkanVertexBuffer {
    public:
        VulkanVertexBuffer();

        ~VulkanVertexBuffer();

        void create(std::shared_ptr<VulkanDevice> device);

        void setVertexData(std::shared_ptr<VulkanDevice> device, std::vector<Vertex> newVertices);

        void destroy(std::shared_ptr<VulkanDevice> device);
    private:
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        std::vector<Vertex> vertices;
        void* bufferMap;
        uint32_t sizeOfCurrentBuffer = 0;
};

#endif