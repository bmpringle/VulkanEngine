#include "Renderer.h"

#include <cstdlib>
#include <cstring>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include <math.h>

Renderer::Renderer(std::shared_ptr<VulkanEngine> engine) : vkEngine(engine) {
    updateVertexBuffer();
    updateInstanceBuffer();
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
        
        std::vector<VkClearValue> clearValues = {{{{0.0f, 0.0f, 0.0f, 1.0f}}}, {{{1.0f, 0}}}};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkEngine->getGraphicsPipeline()->getPipelineLayout(), 0, 1, &vkEngine->getGraphicsPipeline()->getDescriptorSets()[i], 0, nullptr);

        if(sizeOfCurrentBuffer > 0) {
            VkBuffer vertexBuffers[] = {vertexBuffer, instanceBuffer};
            VkDeviceSize offsets[] = {0, 0};
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 2, vertexBuffers, offsets);

            vkCmdDraw(commandBuffers[i], vertices.size(), instanceData.size(), 0, 0);
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

void Renderer::createVertexBuffer() {
    VulkanEngine::createBuffer(sizeof(Vertex) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory, vkEngine->getDevice());
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
        VulkanEngine::createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i], vkEngine->getDevice());
    }

    updateDescriptorSets();
}

glm::mat3x3 calculateXRotationMatrix(double xRotation) {
    glm::mat3x3 rotationMatrix = glm::mat3x3();

    double xRads = xRotation * M_PI / 180;

    rotationMatrix[0][0] = cos(xRads);
    rotationMatrix[0][1] = 0;
    rotationMatrix[0][2] = -sin(xRads);

    rotationMatrix[1][0] = 0;
    rotationMatrix[1][1] = 1;
    rotationMatrix[1][2] = 0;

    rotationMatrix[2][0] = sin(xRads);
    rotationMatrix[2][1] = 0;
    rotationMatrix[2][2] = cos(xRads);

    return rotationMatrix;
}

glm::mat3x3 calculateYRotationMatrix(double yRotation) {

    glm::mat3x3 rotationMatrix = glm::mat3x3(1.0f);

    double yRads = yRotation * M_PI / 180;

    rotationMatrix[0][0] = 1;
    rotationMatrix[0][1] = 0;
    rotationMatrix[0][2] = 0;

    rotationMatrix[1][0] = 0;
    rotationMatrix[1][1] = cos(yRads);
    rotationMatrix[1][2] = sin(yRads);

    rotationMatrix[2][0] = 0;
    rotationMatrix[2][1] = -sin(yRads);
    rotationMatrix[2][2] = cos(yRads);

    return rotationMatrix;
}

glm::mat4x4 createViewMatrix(glm::vec3 camera, float xRotation, float yRotation) {
    glm::mat3x3 rotationMatrixX = calculateXRotationMatrix(-xRotation);
    glm::mat3x3 rotationMatrixY = calculateYRotationMatrix(-yRotation);

    glm::mat3x3 rotationMatrix = rotationMatrixY * rotationMatrixX;

    GLfloat rotationMatrixFloat [16] = {0};

    for(int i = 0; i < 3; ++i) {
        rotationMatrixFloat[4 * i] = rotationMatrix[i][0];
        rotationMatrixFloat[4 * i + 1] = rotationMatrix[i][1];
        rotationMatrixFloat[4 * i + 2] = rotationMatrix[i][2];
        rotationMatrixFloat[4 * i + 3] = 0;
    }
    rotationMatrixFloat[4 * 3] = 0;
    rotationMatrixFloat[4 * 3 + 1] = 0;
    rotationMatrixFloat[4 * 3 + 2] = 0;
    rotationMatrixFloat[4 * 3 + 3] = 1;

    glm::mat4x4 rotationMatrix4x4 = glm::mat4x4(rotationMatrix);

    glm::mat4x4 viewMatrix = glm::mat4x4(1.0f);
    viewMatrix[3] = glm::vec4(glm::vec3(camera.x, camera.y, camera.z), 1);
    viewMatrix = rotationMatrix4x4 * viewMatrix;

    return viewMatrix;
}

void Renderer::updateUniformBuffer(uint32_t imageIndex) {
    bool rotatingBallGoBrrrr = false;

    UniformBuffer ubo{};

    if(rotatingBallGoBrrrr) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        ubo.modelMatrix = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }else {
        ubo.modelMatrix = glm::identity<glm::mat4x4>();
    }

    ubo.viewMatrix = createViewMatrix(camera, xRotation, yRotation);

    /*glm::vec3 direction;
    direction.x = cos(glm::radians(xRotation)) * cos(glm::radians(yRotation));
    direction.y = sin(glm::radians(yRotation));
    direction.z = sin(glm::radians(xRotation)) * cos(glm::radians(yRotation));
    glm::vec3 cameraFront = glm::normalize(direction);

    ubo.viewMatrix = glm::lookAt(camera, camera + cameraFront, glm::vec3(0.0f, 1.0f,  0.0f));*/

    ubo.projectionMatrix = glm::perspective(glm::radians(90.0f), vkEngine->getSwapchain()->getInternalExtent2D().width / (float) vkEngine->getSwapchain()->getInternalExtent2D().height, 0.01f, 100.0f);

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

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = vkEngine->getTextureLoader()->getTextureArrayImageView("game-textures");
        imageInfo.sampler = vkEngine->getTextureLoader()->getTextureSampler();

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = vkEngine->getGraphicsPipeline()->getDescriptorSets()[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = vkEngine->getGraphicsPipeline()->getDescriptorSets()[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(vkEngine->getDevice()->getInternalLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

float& Renderer::getXRotation() {
    return xRotation;
}

float& Renderer::getYRotation() {
    return yRotation;
}

glm::vec3& Renderer::getCameraPosition() {
    return camera;
}

void Renderer::createInstanceBuffer() {
    VulkanEngine::createBuffer(sizeof(InstanceData) * instanceData.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, instanceBuffer, instanceBufferMemory, vkEngine->getDevice());
    vkMapMemory(vkEngine->getDevice()->getInternalLogicalDevice(), instanceBufferMemory, 0, sizeof(InstanceData) * instanceData.size(), 0, &mappingToInstanceBuffer);

    memcpy(mappingToInstanceBuffer, instanceData.data(), (size_t) sizeof(InstanceData) * instanceData.size());
}

void Renderer::updateInstanceBuffer() {
    if(sizeOfCurrentInstanceBuffer == 0) {
        if(instanceData.size() > 0) {
            createInstanceBuffer(); //buffer hasn't been created yet at all
            sizeOfCurrentInstanceBuffer = instanceData.size();
            return;
        }else {
            return; //can't allocate empty buffer. trust me, i tried
        }
    }

    if(sizeOfCurrentInstanceBuffer != instanceData.size()) {
        vkDeviceWaitIdle(vkEngine->getDevice()->getInternalLogicalDevice());
        destroyInstanceBuffer();
        createInstanceBuffer();
    }else {
        memcpy(mappingToInstanceBuffer, instanceData.data(), (size_t) sizeof(InstanceData) * instanceData.size());
    }

    sizeOfCurrentInstanceBuffer = instanceData.size();
}

void Renderer::destroyInstanceBuffer() {
    vkUnmapMemory(vkEngine->getDevice()->getInternalLogicalDevice(), instanceBufferMemory);
    vkDestroyBuffer(vkEngine->getDevice()->getInternalLogicalDevice(), instanceBuffer, nullptr);
    vkFreeMemory(vkEngine->getDevice()->getInternalLogicalDevice(), instanceBufferMemory, nullptr);
}

void Renderer::setInstanceData(std::vector<InstanceData>& newInstanceVertices) {
    instanceData = newInstanceVertices;
    updateInstanceBuffer();
}