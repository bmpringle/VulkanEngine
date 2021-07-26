#ifndef VERTEX_H
#define VERTEX_H

#include "Engine/VulkanInclude.h"

#include <vector>

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        VkVertexInputBindingDescription desc{};
        desc.binding = 0;
        desc.stride = sizeof(Vertex);
        desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return {desc};
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        VkVertexInputAttributeDescription attrib1{};

        attrib1.binding = 0;
        attrib1.location = 0;
        attrib1.format = VK_FORMAT_R32G32B32_SFLOAT; //3 floats vector
        attrib1.offset = offsetof(Vertex, position);

        VkVertexInputAttributeDescription attrib2{};

        attrib2.binding = 0;
        attrib2.location = 1;
        attrib2.format = VK_FORMAT_R32G32B32_SFLOAT; //3 floats vector
        attrib2.offset = offsetof(Vertex, color);

        return {attrib1, attrib2};
    }
};

#endif