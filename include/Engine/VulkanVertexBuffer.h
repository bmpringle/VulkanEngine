#ifndef VULKANVERTEXBUFFER_H
#define VULKANVERTEXBUFFER_H

#include "Vertex.h"

#include "Engine/VulkanEngine.h"

#include <cstring>

template <class VertexType>
class VulkanVertexBuffer {
    public:
        VulkanVertexBuffer() = default;

        ~VulkanVertexBuffer() = default;

        void create(std::shared_ptr<VulkanDevice> device) {
            VulkanEngine::createBuffer(sizeof(VertexType) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory, device);
            vkMapMemory(device->getInternalLogicalDevice(), vertexBufferMemory, 0, sizeof(VertexType) * vertices.size(), 0, &bufferMap);

            std::memcpy(bufferMap, vertices.data(), (size_t) sizeof(VertexType) * vertices.size());
        }

        void setVertexData(std::shared_ptr<VulkanDevice> device, std::vector<VertexType> newVertices) {
            vertices = newVertices;

            if(sizeOfCurrentBuffer == 0) {
                if(vertices.size() > 0) {
                    create(device);
                    sizeOfCurrentBuffer = vertices.size();
                    return;
                }else {
                    return; //can't allocate empty buffer. trust me, i tried
                }
            }

            if(sizeOfCurrentBuffer != vertices.size()) {
                vkDeviceWaitIdle(device->getInternalLogicalDevice());
                destroy(device);
                create(device);
            }else {
                memcpy(bufferMap, vertices.data(), (size_t) sizeof(VertexType) * vertices.size());
            }

            sizeOfCurrentBuffer = vertices.size();
        }

        void destroy(std::shared_ptr<VulkanDevice> device) {
            vkUnmapMemory(device->getInternalLogicalDevice(), vertexBufferMemory);

            vkDestroyBuffer(device->getInternalLogicalDevice(), vertexBuffer, nullptr);

            vkFreeMemory(device->getInternalLogicalDevice(), vertexBufferMemory, nullptr);
        }

        VkBuffer getVertexBuffer() {
            return vertexBuffer;
        }

        uint32_t getBufferSize() {
            return sizeOfCurrentBuffer;
        }
    private:
        VkBuffer vertexBuffer{nullptr};
        VkDeviceMemory vertexBufferMemory;
        std::vector<VertexType> vertices;
        void* bufferMap;
        uint32_t sizeOfCurrentBuffer = 0;
};

#endif