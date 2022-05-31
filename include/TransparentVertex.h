#ifndef TRANSPARENTVERTEX_H
#define TRANSPARENTVERTEX_H

#include "Engine/VulkanInclude.h"

#include <vector>

#include <glm/glm.hpp>

struct TransparentVertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 texCoord;

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        VkVertexInputBindingDescription desc{};
        desc.binding = 0;
        desc.stride = sizeof(TransparentVertex);
        desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkVertexInputBindingDescription desc1{};
        desc1.binding = 1;
        desc1.stride = sizeof(InstanceData);
        desc1.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return {desc, desc1};
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        VkVertexInputAttributeDescription attrib1{};

        attrib1.binding = 0;
        attrib1.location = 0;
        attrib1.format = VK_FORMAT_R32G32B32_SFLOAT; //3 floats vector
        attrib1.offset = offsetof(TransparentVertex, position);

        VkVertexInputAttributeDescription attrib2{};

        attrib2.binding = 0;
        attrib2.location = 1;
        attrib2.format = VK_FORMAT_R32G32B32A32_SFLOAT; //4 floats vector
        attrib2.offset = offsetof(TransparentVertex, color);

        VkVertexInputAttributeDescription attrib3{};

        attrib3.binding = 0;
        attrib3.location = 2;
        attrib3.format = VK_FORMAT_R32G32B32_SFLOAT; //3 floats vector
        attrib3.offset = offsetof(TransparentVertex, texCoord);

        VkVertexInputAttributeDescription attrib4{};

        attrib4.binding = 1;
        attrib4.location = 3;
        attrib4.format = VK_FORMAT_R32G32B32_SFLOAT; //3 floats vector
        attrib4.offset = offsetof(InstanceData, pos);

        return {attrib1, attrib2, attrib3, attrib4};
    }
};

#endif