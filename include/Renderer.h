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

        void setInstanceData(std::vector<InstanceData>& newInstanceVertices);

        float& getXRotation();

        float& getYRotation();

        glm::vec3& getCameraPosition();

    private:
        void createVertexBuffer();

        void updateVertexBuffer();

        void destroyVertexBuffer();

        void createInstanceBuffer();

        void updateInstanceBuffer();

        void destroyInstanceBuffer();

        void destroyUniformBuffers();

        void createUniformBuffers();

        void updateDescriptorSets();

        void updateUniformBuffer(uint32_t imageIndex);

        std::shared_ptr<VulkanEngine> vkEngine;

        std::vector<Vertex> vertices;

        std::vector<InstanceData> instanceData;

        size_t currentFrame = 0;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        VkBuffer instanceBuffer;
        VkDeviceMemory instanceBufferMemory;

        uint32_t sizeOfCurrentBuffer = 0;
        uint32_t sizeOfCurrentInstanceBuffer = 0;

        void* mappingToVertexBuffer;

        void* mappingToInstanceBuffer;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        //in degrees
        float xRotation = 0;
        float yRotation = 0;

        glm::vec3 camera = glm::vec3(0, 4, 0);
};

#endif