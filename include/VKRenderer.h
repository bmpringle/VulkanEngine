#ifndef VKRENDERER_H
#define VKRENDERER_H

#include "Engine/VulkanEngine.h"

#include <memory>

#include "Vertex.h"
#include "OverlayVertex.h"

#include "VulkanVertexBuffer.h"
#include "VulkanUniformBuffer.h"

#include "UniformBuffer.h"
#include "OverlayUniformBuffer.h"

#include <map>

class VKRenderer {
    public:
        VKRenderer(std::shared_ptr<VulkanEngine> engine);

        VKRenderer();

        ~VKRenderer();

        void recordCommandBuffers();

        void renderFrame();

        void setModel(std::string modelID, std::vector<Vertex>& modelVertices);

        void removeModel(std::string modelID);

        void addInstancesToModel(std::string modelID, std::string instanceVectorID, std::vector<InstanceData>& instances);

        void removeInstancesFromModel(std::string modelID, std::string instanceVectorID);

        void setOverlayVertices(std::string id, std::vector<OverlayVertex> newVertices);

        void removeOverlayVertices(std::string id);

        float& getXRotation();

        float& getYRotation();

        glm::vec3& getCameraPosition();

        std::shared_ptr<VulkanEngine> getEngine();

        void addTexture(std::string id, std::string texturePath);
        
        void addTextTexture(std::string id, std::string text);

        void removeTexture(std::string id);

        unsigned int getTextureID(std::string id);

        unsigned int getTextureArrayID(std::string arrayID, std::string textureID);

        std::pair<unsigned int, unsigned int> getTextureDimensions(std::string id);

        std::pair<unsigned int, unsigned int> getTextureArrayDimensions(std::string id);

        void loadTextureArray(std::string id, std::vector<std::string> textures);

        void setCurrentTextureArray(std::string id);

        void setOverlayBounds(float x, float y, float z);

        void setClearColor(glm::vec4 rgba);

        static glm::mat3x3 calculateXRotationMatrix(double xRotation);

        static glm::mat3x3 calculateYRotationMatrix(double xRotation);

        static glm::mat4x4 createViewMatrix(glm::vec3 camera, float xRotation, float yRotation);

    private:
        void createGraphicsPipelines();

        void destroyUniformBuffers();

        void createUniformBuffers();

        void updateDescriptorSets();

        void updateUniformBuffer(uint32_t imageIndex);

        std::shared_ptr<VulkanEngine> vkEngine;

        size_t currentFrame = 0;

        std::map<std::string, std::pair<VulkanVertexBuffer<Vertex>, std::map<std::string, VulkanVertexBuffer<InstanceData>>>> dataIDToVertexData;

        std::map<std::string, VulkanVertexBuffer<OverlayVertex>> dataIDToVertexOverlayData;

        std::vector<VulkanUniformBuffer<UniformBuffer>> blockUniformBuffers;

        std::vector<VulkanUniformBuffer<OverlayUniformBuffer>> overlayUniformBuffers;

        std::map<std::string, std::map<std::string, unsigned int> > texureArrayTexturesToIDs;

        //in degrees
        float xRotation = 0;
        float yRotation = 0;

        glm::vec3 camera = glm::vec3(0, 4, 0);

        OverlayUniformBuffer overlayUBO = {
            {100, 100, 100}
        };

        uint MAX_OVERLAY_TEXTURES = 32;

        std::vector<std::string> overlayTextures = {};

        std::string textureArrayID = "default";

        std::string missingTexture = "assets/missing_texture.png";

        glm::vec4 clearColor = glm::vec4(0, 0, 0, 1);

        std::map<int, std::pair<int, bool*> > canObjectBeDestroyedMap = std::map<int, std::pair<int, bool*> >();
        int mapCounter = 0;
};

#endif