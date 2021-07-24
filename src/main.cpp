/* note: this was created in order to learn vulkan, the tutorial followed was https://vulkan-tutorial.com/
 * as such, while many things will be changed as I write this program, some parts may be very similar or identical.
*/

#include "Renderer.h"

#include <iostream>

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

  while(!engine->getDisplay()->shouldWindowClose()) {
    renderer.recordCommandBuffers();
    renderer.renderFrame();
    glfwPollEvents();
  }

  return 0;
}