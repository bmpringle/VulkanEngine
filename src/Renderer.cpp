#include "Renderer.h"

#include <cstdlib>
#include <cstring>

Renderer::Renderer(std::shared_ptr<VulkanEngine> engine) : vkEngine(engine) {
    updateVertexBuffer();
}

Renderer::~Renderer() {
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

    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
        vkDeviceWaitIdle(device);
        vkEngine->recreateSwapchain();
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
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(Vertex) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vkEngine->getDevice()->getInternalLogicalDevice(), &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkEngine->getDevice()->getInternalLogicalDevice(), vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if(vkAllocateMemory(vkEngine->getDevice()->getInternalLogicalDevice(), &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(vkEngine->getDevice()->getInternalLogicalDevice(), vertexBuffer, vertexBufferMemory, 0);

    vkMapMemory(vkEngine->getDevice()->getInternalLogicalDevice(), vertexBufferMemory, 0, bufferInfo.size, 0, &mappingToVertexBuffer);

    memcpy(mappingToVertexBuffer, vertices.data(), (size_t) bufferInfo.size);
}

void Renderer::destroyVertexBuffer() {
    vkDeviceWaitIdle(vkEngine->getDevice()->getInternalLogicalDevice());
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