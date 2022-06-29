#ifndef COMPOSITEVERTEX_H
#define COMPOSITEVERTEX_H

#include "Engine/VulkanInclude.h"

#include <vector>

#include <glm/glm.hpp>

struct CompositeVertex {
    glm::vec2 pos;

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        VkVertexInputBindingDescription desc{};
        desc.binding = 0;
        desc.stride = sizeof(CompositeVertex);
        desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return {desc};
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        VkVertexInputAttributeDescription attrib1{};

        attrib1.binding = 0;
        attrib1.location = 0;
        attrib1.format = VK_FORMAT_R32G32_SFLOAT; //2 floats vector
        attrib1.offset = offsetof(CompositeVertex, pos);

        return {attrib1};
    }
};

#endif