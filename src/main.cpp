/* note: this was created in order to learn vulkan, the tutorial followed was https://vulkan-tutorial.com/
 * as such, while many things will be changed as I write this program, some parts may be very similar or identical.
*/

#include "Renderer.h"

#include <iostream>

#include <chrono>

std::shared_ptr<VulkanEngine> setupEngine() {
  std::shared_ptr<VulkanEngine> engine = std::make_shared<VulkanEngine>();

  std::shared_ptr<VulkanInstance> instance = std::make_shared<VulkanInstance>();
  instance->setAppName("Test App");
  instance->addValidationLayer("VK_LAYER_KHRONOS_validation");

  std::shared_ptr<VulkanDisplay> display = std::make_shared<VulkanDisplay>();
  display->setInitialWindowDimensions(1000, 800);
  display->setWindowName("Test App Window");

  std::shared_ptr<VulkanDevice> device = std::make_shared<VulkanDevice>();

  std::shared_ptr<VulkanSwapchain> swapchain = std::make_shared<VulkanSwapchain>();

  std::shared_ptr<VulkanRenderSyncObjects> syncObjects = std::make_shared<VulkanRenderSyncObjects>();

  std::shared_ptr<VulkanGraphicsPipeline> graphicsPipeline = std::make_shared<VulkanGraphicsPipeline>();

  graphicsPipeline->setVertexInputBindingDescriptions(Vertex::getBindingDescriptions());
  graphicsPipeline->setVertexInputAttributeDescriptions(Vertex::getAttributeDescriptions());
  graphicsPipeline->setVertexShader("shaders/output/3dvert.spv");
  graphicsPipeline->setFragmentShader("shaders/output/3dfrag.spv");
  graphicsPipeline->addDescriptorSetLayoutBinding(UniformBuffer::getDescriptorSetLayout());

  engine->setInstance(instance);

  engine->setDisplay(display);

  engine->setDevice(device);

  engine->setSwapchain(swapchain);

  engine->setGraphicsPipeline(graphicsPipeline);

  engine->setSyncObjects(syncObjects);

  return engine;
}

int main() {
  std::shared_ptr<VulkanEngine> engine = setupEngine();

  Renderer renderer = Renderer(engine);

  std::vector<Vertex> triangle = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
  };

  std::vector<Vertex> rectangle = {
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},   
    
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},  
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},  
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}
  };

  bool isRenderingTriangle = true;

  std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

  while(!engine->getDisplay()->shouldWindowClose()) {
    if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() > 2000) {
      start = std::chrono::high_resolution_clock::now();

      if(isRenderingTriangle) {
        isRenderingTriangle = false;
        renderer.setVertexData(rectangle);
      }else {
        isRenderingTriangle = true;
        renderer.setVertexData(triangle);
      }
    }
    renderer.recordCommandBuffers();
    renderer.renderFrame();
    glfwPollEvents();
  }

  return 0;
}