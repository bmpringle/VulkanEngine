#include "Renderer.h"

#include <cstdlib>
#include <cstring>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

Renderer::Renderer(std::shared_ptr<VulkanEngine> engine) : vkEngine(engine) {
    updateVertexBuffer();
    createUniformBuffers();
}

Renderer::~Renderer() {
    vkDeviceWaitIdle(vkEngine->getDevice()->getInternalLogicalDevice());
    destroyUniformBuffers();
    destroyVertexBuffer();
}

void Renderer::recordCommandBuffers() {
    std::vector<VkCommandBuffer>& commandBuffers = vkEngine->getSwapchain()->getInternalCommandBuffers();
    std::vector<VkFramebuffer>& swapChainFramebuffers = vkEngine->getSwapchain()->getInternalFramebuffers();
    VkRenderPass& renderPass = vkEngine->getSwapchain()->getInternalRenderPass();
    VkExtent2D& swapChainExtent = vkEngine->getSwapchain()->getInternalExtent2D();
    VkPipeline& graphicsPipeline = vkEngine->getGraphicsPipeline()->getInternalGraphicsPipeline();

    std::vector<VkFence>& imagesInFlight = vkEngine->getSyncObjects()->getInternalImagesInFlight();

    for(size_t i = 0; i < commandBuffers.size(); i++) {
        if(imagesInFlight[i] != VK_NULL_HANDLE) {
            vkWaitForFences(vkEngine->getDevice()->getInternalLogicalDevice(), 1, &imagesInFlight[i], VK_TRUE, UINT64_MAX);
        }

        vkResetCommandBuffer(commandBuffers[i], 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;

        VkClearValue clearColor = {
            {
                {
                    0.0f, 0.0f, 0.0f, 1.0f
                }
            }
        };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkEngine->getGraphicsPipeline()->getPipelineLayout(), 0, 1, &vkEngine->getGraphicsPipeline()->getDescriptorSets()[i], 0, nullptr);

        if(sizeOfCurrentBuffer > 0) {
            VkBuffer vertexBuffers[] = {vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

            vkCmdDraw(commandBuffers[i], vertices.size(), 1, 0, 0);
        }else {
            //nothing to draw
        }

        vkCmdEndRenderPass(commandBuffers[i]);

        if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

void Renderer::renderFrame() {
    std::shared_ptr<VulkanDisplay> vkDisplay = vkEngine->getDisplay();
    std::shared_ptr<VulkanDevice> vkDevice = vkEngine->getDevice();
    std::shared_ptr<VulkanSwapchain> vkSwapchain = vkEngine->getSwapchain();
    std::shared_ptr<VulkanRenderSyncObjects> vkSyncObjects = vkEngine->getSyncObjects();

    VkDevice& device = vkDevice->getInternalLogicalDevice();

    std::vector<VkSemaphore>& imageAvailableSemaphores = vkSyncObjects->getInternalImageAvailableSemaphores();

    std::vector<VkSemaphore>& renderFinishedSemaphores = vkSyncObjects->getInternalImageAvailableSemaphores();

    std::vector<VkFence>& inFlightFences = vkSyncObjects->getInternalInFlightFences();

    std::vector<VkFence>& imagesInFlight = vkSyncObjects->getInternalImagesInFlight();

    std::vector<VkCommandBuffer>& commandBuffers = vkSwapchain->getInternalCommandBuffers();

    VkQueue& graphicsQueue = vkDevice->getInternalGraphicsQueue();
    VkQueue& presentQueue = vkDevice->getInternalPresentQueue();

    VkSwapchainKHR& swapChain = vkSwapchain->getInternalSwapchain();

    int MAX_FRAMES_IN_FLIGHT = vkSyncObjects->getMaxFramesInFlight();

    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    updateUniformBuffer(imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
        vkDeviceWaitIdle(device);
        vkEngine->recreateSwapchain();
        createUniformBuffers();
        return;
    }else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    if(imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional

   result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vkDisplay->getFramebufferResized()) {
        vkDisplay->setFramebufferResized(false);
        vkEngine->recreateSwapchain();
        createUniformBuffers();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

uint32_t Renderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkEngine->getDevice()->getInternalPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("couldn't find memory type that matched the requested properties.");
}

void Renderer::createVertexBuffer() {
    
    createBuffer(sizeof(Vertex) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory);
    vkMapMemory(vkEngine->getDevice()->getInternalLogicalDevice(), vertexBufferMemory, 0, sizeof(Vertex) * vertices.size(), 0, &mappingToVertexBuffer);

    memcpy(mappingToVertexBuffer, vertices.data(), (size_t) sizeof(Vertex) * vertices.size());
}

void Renderer::destroyVertexBuffer() {
    vkUnmapMemory(vkEngine->getDevice()->getInternalLogicalDevice(), vertexBufferMemory);
    vkDestroyBuffer(vkEngine->getDevice()->getInternalLogicalDevice(), vertexBuffer, nullptr);
    vkFreeMemory(vkEngine->getDevice()->getInternalLogicalDevice(), vertexBufferMemory, nullptr);
}

void Renderer::updateVertexBuffer() {
    if(sizeOfCurrentBuffer == 0) {
        if(vertices.size() > 0) {
            createVertexBuffer(); //buffer hasn't been created yet at all
            sizeOfCurrentBuffer = vertices.size();
            return;
        }else {
            return; //can't allocate empty buffer. trust me, i tried
        }
    }

    if(sizeOfCurrentBuffer != vertices.size()) {
        vkDeviceWaitIdle(vkEngine->getDevice()->getInternalLogicalDevice());
        destroyVertexBuffer();
        createVertexBuffer();
    }else {
        memcpy(mappingToVertexBuffer, vertices.data(), (size_t) sizeof(Vertex) * vertices.size());
    }

    sizeOfCurrentBuffer = vertices.size();
}

void Renderer::setVertexData(std::vector<Vertex>& newVertices) {
    vertices = newVertices;
    updateVertexBuffer();
}

void Renderer::destroyUniformBuffers() {
    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        vkDestroyBuffer(vkEngine->getDevice()->getInternalLogicalDevice(), uniformBuffers[i], nullptr);
        vkFreeMemory(vkEngine->getDevice()->getInternalLogicalDevice(), uniformBuffersMemory[i], nullptr);
    }
}

void Renderer::createUniformBuffers() {
    if(uniformBuffers.size() > 0) {
        destroyUniformBuffers();
    }

    VkDeviceSize bufferSize = sizeof(UniformBuffer);

    uniformBuffers.resize(vkEngine->getSwapchain()->getInternalImages().size());
    uniformBuffersMemory.resize(vkEngine->getSwapchain()->getInternalImages().size());

    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
    }

    updateDescriptorSets();
}

void Renderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vkEngine->getDevice()->getInternalLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkEngine->getDevice()->getInternalLogicalDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if(vkAllocateMemory(vkEngine->getDevice()->getInternalLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(vkEngine->getDevice()->getInternalLogicalDevice(), buffer, bufferMemory, 0);
}

void Renderer::updateUniformBuffer(uint32_t imageIndex) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBuffer ubo{};
    ubo.modelMatrix = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.viewMatrix = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.projectionMatrix = glm::perspective(glm::radians(45.0f), vkEngine->getSwapchain()->getInternalExtent2D().width / (float) vkEngine->getSwapchain()->getInternalExtent2D().height, 0.1f, 10.0f);

    ubo.projectionMatrix[1][1] *= -1; // opengl and vulkans y coordinates are flipped, so invert the matrix like this to fix it.

    void* data;
    vkMapMemory(vkEngine->getDevice()->getInternalLogicalDevice(), uniformBuffersMemory[imageIndex], 0, sizeof(ubo), 0, &data);
    
    memcpy(data, &ubo, sizeof(ubo));
    
    vkUnmapMemory(vkEngine->getDevice()->getInternalLogicalDevice(), uniformBuffersMemory[imageIndex]);
}

void Renderer::updateDescriptorSets() {
    for (size_t i = 0; i < vkEngine->getSwapchain()->getInternalImages().size(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBuffer);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = vkEngine->getGraphicsPipeline()->getDescriptorSets()[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(vkEngine->getDevice()->getInternalLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}