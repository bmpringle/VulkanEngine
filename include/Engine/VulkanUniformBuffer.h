#ifndef VULKANUNIFORMBUFFER_H
#define VULKANUNIFORMBUFFER_H

#include "Vertex.h"

#include "Engine/VulkanEngine.h"

#include <cstring>

template <class UniformType>
class VulkanUniformBuffer {
    public:
        VulkanUniformBuffer() = default;

        ~VulkanUniformBuffer() = default;

        void create(std::shared_ptr<VulkanDevice> device) {
            VulkanEngine::createBuffer(sizeof(UniformType), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory, device);
            vkMapMemory(device->getInternalLogicalDevice(), uniformBufferMemory, 0, sizeof(UniformType), 0, &bufferMap);

            hasBeenCreated = true;
        }

        void setVertexData(std::shared_ptr<VulkanDevice> device, UniformType newUniform) {
            uniform = newUniform;

            if(!hasBeenCreated) {
                create(device);
            }

            std::memcpy(bufferMap, &uniform, (size_t) sizeof(UniformType));
        }

        void destroy(std::shared_ptr<VulkanDevice> device) {
            vkUnmapMemory(device->getInternalLogicalDevice(), uniformBufferMemory);

            vkDestroyBuffer(device->getInternalLogicalDevice(), uniformBuffer, nullptr);

            vkFreeMemory(device->getInternalLogicalDevice(), uniformBufferMemory, nullptr);
        }

        VkBuffer getUniformBuffer() {
            return uniformBuffer;
        }

    private:
        VkBuffer uniformBuffer{nullptr};
        VkDeviceMemory uniformBufferMemory;
        UniformType uniform;
        void* bufferMap;
        bool hasBeenCreated = false;
};

#endif