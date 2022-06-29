#ifndef VULKANVERTEXBUFFER_H
#define VULKANVERTEXBUFFER_H

#include "Vertex.h"

#include "Engine/VulkanEngine.h"

#include <cstring>

#include "DeleteThread/DeleteThread.h"

template <class VertexType>
class VulkanVertexBuffer {
    public:
        VulkanVertexBuffer() = default;

        ~VulkanVertexBuffer() = default;

        static void createMemoryHandler(std::shared_ptr<VulkanDevice> device) {
            if(createMemoryHandlers) {
                bufferDeleteFunction = std::bind(&VulkanVertexBuffer::vkBufferDelete, device, std::placeholders::_1);
                bufferDeleteThread = std::make_shared<DeleteThread<VkBuffer>>(bufferDeleteFunction);

                memoryFreeFunction = std::bind(&VulkanVertexBuffer::vkMemoryDelete, device, std::placeholders::_1);
                memoryDeleteThread = std::make_shared<DeleteThread<VkDeviceMemory>>(memoryFreeFunction);

                createMemoryHandlers = false;
            }
        }

        void create(std::shared_ptr<VulkanDevice> device) {
            if(createMemoryHandlers) {
                bufferDeleteFunction = std::bind(&VulkanVertexBuffer::vkBufferDelete, device, std::placeholders::_1);
                bufferDeleteThread = std::make_shared<DeleteThread<VkBuffer>>(bufferDeleteFunction);

                memoryFreeFunction = std::bind(&VulkanVertexBuffer::vkMemoryDelete, device, std::placeholders::_1);
                memoryDeleteThread = std::make_shared<DeleteThread<VkDeviceMemory>>(memoryFreeFunction);

                createMemoryHandlers = false;
            }

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
                bool temp = true;
                destroy(device, &temp); //ok because ptr isn't stored
                create(device);
            }else {
                memcpy(bufferMap, vertices.data(), (size_t) sizeof(VertexType) * vertices.size());
            }

            sizeOfCurrentBuffer = vertices.size();
        }

        void destroy(std::shared_ptr<VulkanDevice> device, bool* deleteOldBufferBool) {
            if(vertexBufferMemory != VK_NULL_HANDLE && vertexBuffer != VK_NULL_HANDLE) {
                vkUnmapMemory(device->getInternalLogicalDevice(), vertexBufferMemory);

                if(*deleteOldBufferBool) {
                    vkDestroyBuffer(device->getInternalLogicalDevice(), vertexBuffer, nullptr);
                    vkFreeMemory(device->getInternalLogicalDevice(), vertexBufferMemory, nullptr);
                }else {
                    bufferDeleteThread->addObjectToDelete(vertexBuffer, deleteOldBufferBool);
                    memoryDeleteThread->addObjectToDelete(vertexBufferMemory, deleteOldBufferBool);
                }
            }else {
                if(vertices.size() != 0) {
                    std::cout << "invalid vkbuffer or vkbuffermemory pointers used when trying to delete vulkanvertexbuffer" << std::endl;
                }else {
                    //nothing to delete b/c the handles are NULL. this is ok when vertices.size() == 0 b/c it just means that there was never any geometry set in the buffer so it was never created
                }
            }
        }

        VkBuffer getVertexBuffer() {
            return vertexBuffer;
        }

        uint32_t getBufferSize() {
            return sizeOfCurrentBuffer;
        }

        static void forceJoinDeleteThreads() {
            if(bufferDeleteThread != nullptr) {
                bufferDeleteThread->forceJoin();
            }
           
            if(memoryDeleteThread != nullptr) {
                memoryDeleteThread->forceJoin();
            }   
        }

        static std::pair<std::mutex*, std::mutex*> getDeleteFunctionMutexes() {
            return std::make_pair<std::mutex*, std::mutex*>(bufferDeleteThread->getMutexPointer(), memoryDeleteThread->getMutexPointer());
        }

    private:
        static bool createMemoryHandlers;
        VkBuffer vertexBuffer{nullptr};
        VkDeviceMemory vertexBufferMemory;
        std::vector<VertexType> vertices;
        void* bufferMap;
        uint32_t sizeOfCurrentBuffer = 0;

        static void vkBufferDelete(std::shared_ptr<VulkanDevice> device, VkBuffer buffer) {
            vkDestroyBuffer(device->getInternalLogicalDevice(), buffer, nullptr);
        }

        static void vkMemoryDelete(std::shared_ptr<VulkanDevice> device, VkDeviceMemory memory) {
            vkFreeMemory(device->getInternalLogicalDevice(), memory, nullptr);
        }

        static std::function<void(VkBuffer)> bufferDeleteFunction;

        static std::function<void(VkDeviceMemory)> memoryFreeFunction;

        static std::shared_ptr<DeleteThread<VkBuffer>> bufferDeleteThread;

        static std::shared_ptr<DeleteThread<VkDeviceMemory>> memoryDeleteThread;
};

template <class VertexType>
bool VulkanVertexBuffer<VertexType>::createMemoryHandlers = true;

//set to nullptr for now to get rid of compile errors. so long as it isn't used before at least one VulkanVertexBuffer has create() called on it this is fine
template <class VertexType>
std::function<void(VkBuffer)> VulkanVertexBuffer<VertexType>::bufferDeleteFunction = nullptr;

template <class VertexType>
std::function<void(VkDeviceMemory)> VulkanVertexBuffer<VertexType>::memoryFreeFunction = nullptr;

template <class VertexType>
std::shared_ptr<DeleteThread<VkBuffer>> VulkanVertexBuffer<VertexType>::bufferDeleteThread = nullptr; 

template <class VertexType>
std::shared_ptr<DeleteThread<VkDeviceMemory>> VulkanVertexBuffer<VertexType>::memoryDeleteThread = nullptr;

#endif