#include "VulkanVertexBuffer.h"

#include <cstring>

VulkanVertexBuffer::VulkanVertexBuffer() {

}

VulkanVertexBuffer::~VulkanVertexBuffer() {
    
}

void VulkanVertexBuffer::create(std::shared_ptr<VulkanDevice> device) {
    VulkanEngine::createBuffer(sizeof(Vertex) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory, device);
    vkMapMemory(device->getInternalLogicalDevice(), vertexBufferMemory, 0, sizeof(Vertex) * vertices.size(), 0, &bufferMap);

    std::memcpy(bufferMap, vertices.data(), (size_t) sizeof(Vertex) * vertices.size());
}

void VulkanVertexBuffer::setVertexData(std::shared_ptr<VulkanDevice> device, std::vector<Vertex> newVertices) {
    vertices = newVertices;
    
    if(sizeOfCurrentBuffer == 0) {
        if(vertices.size() > 0) {
            create(device);
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
        memcpy(bufferMap, vertices.data(), (size_t) sizeof(Vertex) * vertices.size());
    }

    sizeOfCurrentBuffer = vertices.size();
}

void VulkanVertexBuffer::destroy(std::shared_ptr<VulkanDevice> device) {
    vkUnmapMemory(device->getInternalLogicalDevice(), vertexBufferMemory);

    vkDestroyBuffer(device->getInternalLogicalDevice(), vertexBuffer, nullptr);

    vkFreeMemory(device->getInternalLogicalDevice(), vertexBufferMemory, nullptr);
}