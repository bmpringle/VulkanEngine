#ifndef WIREFRAMEVERTEX_H
#define WIREFRAMEVERTEX_H

#include "Engine/VulkanInclude.h"

#include <vector>

#include <glm/glm.hpp>

#include "Vertex.h"

struct WireframeVertex {
    glm::vec3 position;

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        VkVertexInputBindingDescription desc{};
        desc.binding = 0;
        desc.stride = sizeof(WireframeVertex);
        desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkVertexInputBindingDescription desc1{};
        desc1.binding = 1;
        desc1.stride = sizeof(WireframeVertex);
        desc1.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return {desc, desc1};
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        VkVertexInputAttributeDescription attrib1{};

        attrib1.binding = 0;
        attrib1.location = 0;
        attrib1.format = VK_FORMAT_R32G32B32_SFLOAT; //3 floats vector
        attrib1.offset = offsetof(WireframeVertex, position);

        VkVertexInputAttributeDescription attrib2{};

        attrib2.binding = 1;
        attrib2.location = 1;
        attrib2.format = VK_FORMAT_R32G32B32_SFLOAT; //3 floats vector
        attrib2.offset = offsetof(InstanceData, pos);

        return {attrib1, attrib2};
    }
};

#endif