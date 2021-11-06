#ifndef OVERLAYVERTEX_H
#define OVERLAYVERTEX_H

#include "Engine/VulkanInclude.h"

#include <vector>

#include <glm/glm.hpp>

struct OverlayVertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;
    uint32_t texID;

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        VkVertexInputBindingDescription desc{};
        desc.binding = 0;
        desc.stride = sizeof(OverlayVertex);
        desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return {desc};
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        VkVertexInputAttributeDescription attrib1{};

        attrib1.binding = 0;
        attrib1.location = 0;
        attrib1.format = VK_FORMAT_R32G32B32_SFLOAT; //3 floats vector
        attrib1.offset = offsetof(OverlayVertex, position);

        VkVertexInputAttributeDescription attrib2{};

        attrib2.binding = 0;
        attrib2.location = 1;
        attrib2.format = VK_FORMAT_R32G32B32_SFLOAT; //3 floats vector
        attrib2.offset = offsetof(OverlayVertex, color);

        VkVertexInputAttributeDescription attrib3{};

        attrib3.binding = 0;
        attrib3.location = 2;
        attrib3.format = VK_FORMAT_R32G32_SFLOAT; //2 floats vector
        attrib3.offset = offsetof(OverlayVertex, texCoord);

        VkVertexInputAttributeDescription attrib4{};

        attrib4.binding = 0;
        attrib4.location = 3;
        attrib4.format = VK_FORMAT_R32_UINT; //1 uint
        attrib4.offset = offsetof(OverlayVertex, texID);

        return {attrib1, attrib2, attrib3, attrib4};
    }
};

#endif