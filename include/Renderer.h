#ifndef RENDERER_H
#define RENDERER_H

#include "Engine/VulkanEngine.h"

#include <memory>

#include "Vertex.h"
#include "UniformBuffer.h"

class Renderer {
    public:
        Renderer(std::shared_ptr<VulkanEngine> engine);

        ~Renderer();

        void recordCommandBuffers();

        void renderFrame();

        void setVertexData(std::vector<Vertex>& newVertices);

        float& getXRotation();

        float& getYRotation();

        glm::vec3& getCameraPosition();

    private:
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        void createVertexBuffer();

        void updateVertexBuffer();

        void destroyVertexBuffer();

        void destroyUniformBuffers();

        void createUniformBuffers();

        void updateDescriptorSets();

        void updateUniformBuffer(uint32_t imageIndex);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        std::shared_ptr<VulkanEngine> vkEngine;

        std::vector<Vertex> vertices;

        size_t currentFrame = 0;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        uint32_t sizeOfCurrentBuffer = 0;

        void* mappingToVertexBuffer;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        //in degrees
        float xRotation = 0;
        float yRotation = 0;

        glm::vec3 camera = glm::vec3(2, 0, -3);
};

#endif