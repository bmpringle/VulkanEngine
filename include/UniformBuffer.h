#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include <glm/glm.hpp>

#include "Engine/VulkanInclude.h"

struct UniformBuffer {
    alignas(16) glm::mat4x4 modelMatrix;
    alignas(16) glm::mat4x4 viewMatrix;
    alignas(16) glm::mat4x4 projectionMatrix;
    alignas(16) glm::vec3 tint;
    
    static VkDescriptorSetLayoutBinding getDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        return uboLayoutBinding;
    }
};

#endif