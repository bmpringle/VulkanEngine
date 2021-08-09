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
    createUniformBuffers();
}

Renderer::Renderer() : vkEngine(std::make_shared<VulkanEngine>()) {
    std::shared_ptr<VulkanInstance> instance = std::make_shared<VulkanInstance>();
    instance->setAppName("Test App");
    instance->addValidationLayer("VK_LAYER_KHRONOS_validation");

    std::shared_ptr<VulkanDisplay> display = std::make_shared<VulkanDisplay>();
    display->setInitialWindowDimensions(1000, 800);
    display->setWindowName("Test App Window");

    vkEngine->setInstance(instance);

    vkEngine->setDisplay(display);

    std::shared_ptr<VulkanDevice> device = std::make_shared<VulkanDevice>();

    device->addDeviceExtension("VK_EXT_descriptor_indexing");

    vkEngine->setDevice(device);

    std::shared_ptr<VulkanSwapchain> swapchain = std::make_shared<VulkanSwapchain>();

    vkEngine->setSwapchain(swapchain);

    std::shared_ptr<VulkanRenderSyncObjects> syncObjects = std::make_shared<VulkanRenderSyncObjects>();

    vkEngine->setSyncObjects(syncObjects);

    createGraphicsPipelines();

    std::shared_ptr<TextureLoader> textureLoader = vkEngine->getTextureLoader();

    textureLoader->loadTexture(vkEngine->getDevice(), "missing_texture", "assets/missing_texture.png");

    textureLoader->loadTextureArray(device, {"assets/dirt.png", "assets/grass_side.png"}, "game-textures");

    createUniformBuffers();
}

Renderer::~Renderer() {
    vkDeviceWaitIdle(vkEngine->getDevice()->getInternalLogicalDevice());
    destroyUniformBuffers();

    for(std::pair<const std::string, std::pair<VulkanVertexBuffer<Vertex>, VulkanVertexBuffer<InstanceData>>> vertexData : dataIDToVertexData) {
        vertexData.second.first.destroy(vkEngine->getDevice());
        vertexData.second.second.destroy(vkEngine->getDevice());
    }

    for(std::pair<const std::string, VulkanVertexBuffer<OverlayVertex>> vertexData : dataIDToVertexOverlayData) {
        vertexData.second.destroy(vkEngine->getDevice());
    }
}

void Renderer::recordCommandBuffers() {
    std::vector<VkCommandBuffer>& commandBuffers = vkEngine->getSwapchain()->getInternalCommandBuffers();
    std::vector<VkFramebuffer>& swapChainFramebuffers = vkEngine->getSwapchain()->getInternalFramebuffers();
    VkRenderPass& renderPass = vkEngine->getSwapchain()->getInternalRenderPass();
    VkExtent2D& swapChainExtent = vkEngine->getSwapchain()->getInternalExtent2D();

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


        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkEngine->getGraphicsPipeline(0)->getInternalGraphicsPipeline());

        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkEngine->getGraphicsPipeline(0)->getPipelineLayout(), 0, 1, &vkEngine->getGraphicsPipeline(0)->getDescriptorSets()[i], 0, nullptr);

        for(std::pair<const std::string, std::pair<VulkanVertexBuffer<Vertex>, VulkanVertexBuffer<InstanceData>>> vertexData : dataIDToVertexData) {
            VulkanVertexBuffer<Vertex>& vertexBuffer = vertexData.second.first;
            VulkanVertexBuffer<InstanceData>& instanceBuffer = vertexData.second.second;

            if(vertexBuffer.getBufferSize() > 0 && instanceBuffer.getBufferSize() > 0) {
                VkBuffer vertexBuffers[] = {vertexBuffer.getVertexBuffer(), instanceBuffer.getVertexBuffer()};
                VkDeviceSize offsets[] = {0, 0};
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 2, vertexBuffers, offsets);

                vkCmdDraw(commandBuffers[i], vertexBuffer.getBufferSize(), instanceBuffer.getBufferSize(), 0, 0);
            }else {
                //nothing to draw
            }
        }


        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkEngine->getGraphicsPipeline(1)->getInternalGraphicsPipeline());
        
        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkEngine->getGraphicsPipeline(1)->getPipelineLayout(), 0, 1, &vkEngine->getGraphicsPipeline(1)->getDescriptorSets()[i], 0, nullptr);
        for(std::pair<const std::string, VulkanVertexBuffer<OverlayVertex>> vertexData : dataIDToVertexOverlayData) {
            VulkanVertexBuffer<OverlayVertex> vertexBuffer = vertexData.second;

            if(vertexBuffer.getBufferSize() > 0) {
                VkBuffer vertexBuffers[] = {vertexBuffer.getVertexBuffer()};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

                vkCmdDraw(commandBuffers[i], vertexBuffer.getBufferSize(), 1, 0, 0);
            }
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

void Renderer::destroyUniformBuffers() {
    for (size_t i = 0; i < blockUniformBuffers.size(); i++) {
        blockUniformBuffers.at(i).destroy(vkEngine->getDevice());
    }

    for (size_t i = 0; i < overlayUniformBuffers.size(); i++) {
        overlayUniformBuffers.at(i).destroy(vkEngine->getDevice());
    }
}

void Renderer::createUniformBuffers() {
    if(blockUniformBuffers.size() > 0) {
        destroyUniformBuffers();
    }

    blockUniformBuffers.resize(vkEngine->getSwapchain()->getInternalImages().size());
    overlayUniformBuffers.resize(vkEngine->getSwapchain()->getInternalImages().size());


    for (size_t i = 0; i < blockUniformBuffers.size(); i++) {
        blockUniformBuffers.at(i).create(vkEngine->getDevice());
    }

    for (size_t i = 0; i < blockUniformBuffers.size(); i++) {
        overlayUniformBuffers.at(i).create(vkEngine->getDevice());
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

    blockUniformBuffers.at(imageIndex).setVertexData(vkEngine->getDevice(), ubo);

    overlayUniformBuffers.at(imageIndex).setVertexData(vkEngine->getDevice(), overlayUBO);
}

void Renderer::updateDescriptorSets() {
    bool recreateGraphicsPipelines = false;

    while(overlayTextures.size() > MAX_OVERLAY_TEXTURES) {
        MAX_OVERLAY_TEXTURES = MAX_OVERLAY_TEXTURES * 2;

        recreateGraphicsPipelines = true;
    }

    if(recreateGraphicsPipelines) {
        createGraphicsPipelines();
    }

    for (size_t i = 0; i < vkEngine->getSwapchain()->getInternalImages().size(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = blockUniformBuffers.at(i).getUniformBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBuffer);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = vkEngine->getTextureLoader()->getTextureArrayImageView("game-textures");
        imageInfo.sampler = vkEngine->getTextureLoader()->getTextureSampler();

        std::array<VkWriteDescriptorSet, 4> descriptorWrites{};

        //descriptor writes for block pipeline
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = vkEngine->getGraphicsPipeline(0)->getDescriptorSets()[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = vkEngine->getGraphicsPipeline(0)->getDescriptorSetLayoutBinding(0).descriptorType;
        descriptorWrites[0].descriptorCount = vkEngine->getGraphicsPipeline(0)->getDescriptorSetLayoutBinding(0).descriptorCount;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = vkEngine->getGraphicsPipeline(0)->getDescriptorSets()[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = vkEngine->getGraphicsPipeline(0)->getDescriptorSetLayoutBinding(1).descriptorType;
        descriptorWrites[1].descriptorCount = vkEngine->getGraphicsPipeline(0)->getDescriptorSetLayoutBinding(1).descriptorCount;
        descriptorWrites[1].pImageInfo = &imageInfo;

        //descriptor writes for overlay pipeline
        std::vector<VkDescriptorImageInfo> imageInfos{}; 

        for(std::string& texture : overlayTextures) {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = vkEngine->getTextureLoader()->getImageView(texture);
            imageInfo.sampler = vkEngine->getTextureLoader()->getTextureSampler();

            imageInfos.push_back(imageInfo);
        }

        for(int i = overlayTextures.size(); i < MAX_OVERLAY_TEXTURES; ++i) {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = vkEngine->getTextureLoader()->getImageView("missing_texture");
            imageInfo.sampler = vkEngine->getTextureLoader()->getTextureSampler();

            imageInfos.push_back(imageInfo);
        }

        VkDescriptorBufferInfo bufferInfoOverlay{};
        bufferInfoOverlay.buffer = overlayUniformBuffers.at(i).getUniformBuffer();
        bufferInfoOverlay.offset = 0;
        bufferInfoOverlay.range = sizeof(OverlayUniformBuffer);

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = vkEngine->getGraphicsPipeline(1)->getDescriptorSets()[i];
        descriptorWrites[2].dstBinding = 0;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = vkEngine->getGraphicsPipeline(1)->getDescriptorSetLayoutBinding(0).descriptorType;
        descriptorWrites[2].descriptorCount = vkEngine->getGraphicsPipeline(1)->getDescriptorSetLayoutBinding(0).descriptorCount;
        descriptorWrites[2].pBufferInfo = &bufferInfoOverlay;

        descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[3].dstSet = vkEngine->getGraphicsPipeline(1)->getDescriptorSets()[i];
        descriptorWrites[3].dstBinding = 1;
        descriptorWrites[3].dstArrayElement = 0;
        descriptorWrites[3].descriptorType = vkEngine->getGraphicsPipeline(1)->getDescriptorSetLayoutBinding(1).descriptorType;
        descriptorWrites[3].descriptorCount = vkEngine->getGraphicsPipeline(1)->getDescriptorSetLayoutBinding(1).descriptorCount;
        descriptorWrites[3].pImageInfo = imageInfos.data();

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

void Renderer::setDataPair(std::string id, std::vector<Vertex>& newVertices, std::vector<InstanceData>& newInstanceVertices) {
    if(dataIDToVertexData.count(id) > 0) {
        dataIDToVertexData[id].first.setVertexData(vkEngine->getDevice(), newVertices);
        dataIDToVertexData[id].second.setVertexData(vkEngine->getDevice(), newInstanceVertices);
        return;
    }

    VulkanVertexBuffer<Vertex> vertexBuffer = VulkanVertexBuffer<Vertex>();
    vertexBuffer.setVertexData(vkEngine->getDevice(), newVertices);

    VulkanVertexBuffer<InstanceData> instanceBuffer = VulkanVertexBuffer<InstanceData>();
    instanceBuffer.setVertexData(vkEngine->getDevice(), newInstanceVertices);

    dataIDToVertexData[id] = std::pair<VulkanVertexBuffer<Vertex>, VulkanVertexBuffer<InstanceData>>(vertexBuffer, instanceBuffer);
}

void Renderer::setOverlayVertices(std::string id, std::vector<OverlayVertex> newVertices) {
    if(dataIDToVertexOverlayData.count(id) > 0) {
        dataIDToVertexOverlayData[id].setVertexData(vkEngine->getDevice(), newVertices);
        return;
    }

    VulkanVertexBuffer<OverlayVertex> vertexBuffer = VulkanVertexBuffer<OverlayVertex>();
    vertexBuffer.setVertexData(vkEngine->getDevice(), newVertices);

    dataIDToVertexOverlayData[id] = vertexBuffer;
}

std::shared_ptr<VulkanEngine> Renderer::getEngine() {
    return vkEngine;
}

void Renderer::addTexture(std::string id, std::string texturePath) {
    vkEngine->getTextureLoader()->loadTexture(vkEngine->getDevice(), id, texturePath);

    if(std::find(overlayTextures.begin(), overlayTextures.end(), id) == overlayTextures.end()) {
        overlayTextures.push_back(id);
    }

    updateDescriptorSets();
}

void Renderer::addTextTexture(std::string id, std::string text) {
    vkEngine->getTextureLoader()->loadTextToTexture(vkEngine->getDevice(), id, text);

    if(std::find(overlayTextures.begin(), overlayTextures.end(), id) == overlayTextures.end()) {
        overlayTextures.push_back(id);
    }
    
    updateDescriptorSets();
}

void Renderer::removeTexture(std::string id) {
    auto iter = std::find(overlayTextures.begin(), overlayTextures.end(), id);
    if(iter == overlayTextures.end()) {
        throw std::runtime_error("couldnt find " + id + " in overlayTextures!");
    }

    overlayTextures.erase(iter);
}

unsigned int Renderer::getTextureID(std::string id) {
    auto iter = std::find(overlayTextures.begin(), overlayTextures.end(), id);
    if(iter == overlayTextures.end()) {
        throw std::runtime_error("couldnt find " + id + " in overlayTextures!");
    }
    return iter - overlayTextures.begin();
}

void Renderer::createGraphicsPipelines() {
    std::shared_ptr<VulkanSwapchain> swapchain = vkEngine->getSwapchain();

    std::shared_ptr<VulkanGraphicsPipeline> graphicsPipelineBlocks = std::make_shared<VulkanGraphicsPipeline>();

    graphicsPipelineBlocks->setVertexInputBindingDescriptions(Vertex::getBindingDescriptions());
    graphicsPipelineBlocks->setVertexInputAttributeDescriptions(Vertex::getAttributeDescriptions());
    graphicsPipelineBlocks->setVertexShader("shaders/output/3dvert_instanced_texArray.spv");
    graphicsPipelineBlocks->setFragmentShader("shaders/output/3dfrag_instanced_texArray.spv");
    graphicsPipelineBlocks->addDescriptorSetLayoutBinding(UniformBuffer::getDescriptorSetLayout());

    //texture array binding
    VkDescriptorSetLayoutBinding textureArrayLayoutBinding{};
    textureArrayLayoutBinding.binding = 1;
    textureArrayLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureArrayLayoutBinding.descriptorCount = 1;
    textureArrayLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    textureArrayLayoutBinding.pImmutableSamplers = nullptr;

    graphicsPipelineBlocks->addDescriptorSetLayoutBinding(textureArrayLayoutBinding);

    graphicsPipelineBlocks->setDescriptorPoolData(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapchain->getInternalImages().size());

    graphicsPipelineBlocks->setDescriptorPoolData(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, swapchain->getInternalImages().size());

    vkEngine->setGraphicsPipeline(graphicsPipelineBlocks, 0);


    std::shared_ptr<VulkanGraphicsPipeline> graphicsPipelineOverlays = std::make_shared<VulkanGraphicsPipeline>();

    graphicsPipelineOverlays->setVertexInputBindingDescriptions(OverlayVertex::getBindingDescriptions());
    graphicsPipelineOverlays->setVertexInputAttributeDescriptions(OverlayVertex::getAttributeDescriptions());
    graphicsPipelineOverlays->setVertexShader("shaders/output/vert_overlay.spv");
    graphicsPipelineOverlays->setFragmentShader("shaders/output/frag_overlay.spv");
    graphicsPipelineOverlays->addDescriptorSetLayoutBinding(OverlayUniformBuffer::getDescriptorSetLayout());

    //array of textures binding
    VkDescriptorSetLayoutBinding arrayOfTexturesLayoutBinding{};
    arrayOfTexturesLayoutBinding.binding = 1;
    arrayOfTexturesLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    arrayOfTexturesLayoutBinding.descriptorCount = MAX_OVERLAY_TEXTURES;
    arrayOfTexturesLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    arrayOfTexturesLayoutBinding.pImmutableSamplers = nullptr;

    graphicsPipelineOverlays->setDescriptorPoolFlags(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT);

    graphicsPipelineOverlays->setDescriptorSetLayoutFlags(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT);

    graphicsPipelineOverlays->addDescriptorSetLayoutBinding(arrayOfTexturesLayoutBinding);

    graphicsPipelineOverlays->setDescriptorPoolData(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapchain->getInternalImages().size());

    graphicsPipelineOverlays->setDescriptorPoolData(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, swapchain->getInternalImages().size() * MAX_OVERLAY_TEXTURES);

    vkEngine->setGraphicsPipeline(graphicsPipelineOverlays, 1);
}

std::pair<unsigned int, unsigned int> Renderer::getTextureDimensions(std::string id) {
    return vkEngine->getTextureLoader()->getTextureDimensions(id);
}

std::pair<unsigned int, unsigned int> Renderer::getTextureArrayDimensions(std::string id) {
    return vkEngine->getTextureLoader()->getTextureArrayDimensions(id);
}