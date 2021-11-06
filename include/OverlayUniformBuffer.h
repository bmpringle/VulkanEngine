#ifndef OVERLAYUNIFORMBUFFER_H
#define OVERLAYUNIFORMBUFFER_H

#include <glm/glm.hpp>

#include "Engine/VulkanInclude.h"

struct OverlayUniformBuffer {
    alignas(16) glm::vec3 bounds;

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