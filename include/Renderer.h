#ifndef RENDERER_H
#define RENDERER_H

#include "Engine/VulkanEngine.h"

#include <memory>

#include "Vertex.h"
#include "OverlayVertex.h"

#include "VulkanVertexBuffer.h"
#include "VulkanUniformBuffer.h"

#include "UniformBuffer.h"
#include "OverlayUniformBuffer.h"

class Renderer {
    public:
        Renderer(std::shared_ptr<VulkanEngine> engine);

        Renderer();

        ~Renderer();

        void recordCommandBuffers();

        void renderFrame();

        void setDataPair(std::string id, std::vector<Vertex>& newVertices, std::vector<InstanceData>& newInstanceVertices);

        void setOverlayVertices(std::string id, std::vector<OverlayVertex> newVertices);

        float& getXRotation();

        float& getYRotation();

        glm::vec3& getCameraPosition();

        std::shared_ptr<VulkanEngine> getEngine();

        void addTexture(std::string id, std::string texturePath);
        
        void addTextTexture(std::string id, std::string text);

    private:
        void destroyUniformBuffers();

        void createUniformBuffers();

        void updateDescriptorSets();

        void updateUniformBuffer(uint32_t imageIndex);

        std::shared_ptr<VulkanEngine> vkEngine;

        size_t currentFrame = 0;

        std::map<std::string, std::pair<VulkanVertexBuffer<Vertex>, VulkanVertexBuffer<InstanceData>>> dataIDToVertexData;

        std::map<std::string, VulkanVertexBuffer<OverlayVertex>> dataIDToVertexOverlayData;

        std::vector<VulkanUniformBuffer<UniformBuffer>> blockUniformBuffers;

        std::vector<VulkanUniformBuffer<OverlayUniformBuffer>> overlayUniformBuffers;

        //in degrees
        float xRotation = 0;
        float yRotation = 0;

        glm::vec3 camera = glm::vec3(0, 4, 0);

        OverlayUniformBuffer overlayUBO = {
            {100, 100}
        };

        const int MAX_OVERLAY_TEXTURES = 10;

        std::vector<std::string> overlayTextures = {"assets/test.jpg", "assets/cube-cube.png", "assets/dirt.png", "assets/test-OLD.jpg", "assets/grass_side.png"};


        std::string missingTexture = "assets/missing_texture.png";
};

#endif