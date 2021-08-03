#ifndef RENDERER_H
#define RENDERER_H

#include "Engine/VulkanEngine.h"

#include <memory>

#include "Vertex.h"
#include "UniformBuffer.h"

#include "VulkanVertexBuffer.h"

class Renderer {
    public:
        Renderer(std::shared_ptr<VulkanEngine> engine);

        ~Renderer();

        void recordCommandBuffers();

        void renderFrame();

        void setDataPair(std::string id, std::vector<Vertex>& newVertices, std::vector<InstanceData>& newInstanceVertices);

        float& getXRotation();

        float& getYRotation();

        glm::vec3& getCameraPosition();

    private:
        void destroyUniformBuffers();

        void createUniformBuffers();

        void updateDescriptorSets();

        void updateUniformBuffer(uint32_t imageIndex);

        std::shared_ptr<VulkanEngine> vkEngine;

        size_t currentFrame = 0;

        std::map<std::string, std::pair<VulkanVertexBuffer<Vertex>, VulkanVertexBuffer<InstanceData>>> dataIDToVertexData;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        //in degrees
        float xRotation = 0;
        float yRotation = 0;

        glm::vec3 camera = glm::vec3(0, 4, 0);
};

#endif