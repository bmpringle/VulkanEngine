/* note: this was created in order to learn vulkan, the tutorial followed was https://vulkan-tutorial.com/
 * as such, while many things will be changed as I write this program, some parts may be very similar or identical.
*/

#include "VulkanApp.h"

#include <iostream>

#include "VulkanEngine.h"

int main() {
  /*VulkanApp app = VulkanApp();

  app.run();*/

  VulkanEngine engine = VulkanEngine();

  VulkanInstance instance = VulkanInstance();
  instance.setAppName("Test App");
  instance.addValidationLayer("VK_LAYER_KHRONOS_validation");

  engine.setInstance(instance);

  VulkanDisplay display = VulkanDisplay();
  display.setInitialWindowDimensions(1000, 800);
  display.setWindowName("Test App Window");

  engine.setDisplay(display);

  VulkanDevice device = VulkanDevice();

  engine.setDevice(device);

  VulkanSwapchain swapchain = VulkanSwapchain();

  engine.setSwapchain(swapchain);

  while(!engine.getDisplay().shouldWindowClose()) {
    engine.engineLoop();
  }
  
  return 0;
}