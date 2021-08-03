/* note: this was created in order to learn vulkan, the tutorial followed was https://vulkan-tutorial.com/
 * as such, while many things will be changed as I write this program, some parts may be very similar or identical.
*/

#include "Renderer.h"

#include <iostream>

#include <chrono>

#include <glm/gtx/string_cast.hpp>

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
  graphicsPipeline->setVertexShader("shaders/output/3dvert_instanced_texArray.spv");
  graphicsPipeline->setFragmentShader("shaders/output/3dfrag_instanced_texArray.spv");
  graphicsPipeline->addDescriptorSetLayoutBinding(UniformBuffer::getDescriptorSetLayout());

  engine->setInstance(instance);

  engine->setDisplay(display);

  engine->setDevice(device);

  engine->setSwapchain(swapchain);

  engine->setGraphicsPipeline(graphicsPipeline);

  engine->setSyncObjects(syncObjects);

  std::shared_ptr<TextureLoader> textureLoader = engine->getTextureLoader();
  textureLoader->loadTexture(device, "assets/test.jpg");
  textureLoader->loadTexture(device, "assets/cube-cube.png");
  textureLoader->loadTexture(device, "assets/cube-cube-cube.png");
  textureLoader->loadTextureArray(device, {"assets/dirt.png", "assets/grass_side.png"}, "game-textures");

  return engine;
}

//-1 means it hasn't been set yet
double previousMouseX = -1;
double previousMouseY = -1;

float sensitivity = 0.1;

float xDelta = 0;
float yDelta = 0;

void glfwMouseCallback(GLFWwindow* window, double mouseX, double mouseY) {
  if(previousMouseX != -1) {
    xDelta = (previousMouseX - mouseX) * sensitivity;
  }
  previousMouseX = mouseX;

  if(previousMouseY != -1) {
    yDelta = (previousMouseY - mouseY) * sensitivity;
  }
  previousMouseY = mouseY;
}

bool w_pressed = false;
bool a_pressed = false;
bool s_pressed = false;
bool d_pressed = false;
bool up_key_pressed = false;
bool down_key_pressed = false;

void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS) {
    if(key == GLFW_KEY_W) {
      w_pressed = true;
    }else if(key == GLFW_KEY_A) {
      a_pressed = true;
    }else if(key == GLFW_KEY_S) {
      s_pressed = true;
    }else if(key == GLFW_KEY_D) {
      d_pressed = true;
    }else if(key == GLFW_KEY_UP) {
      up_key_pressed = true;
    }else if(key == GLFW_KEY_DOWN) {
      down_key_pressed = true;
    }
  }else if(action == GLFW_RELEASE) {
    if(key == GLFW_KEY_W) {
      w_pressed = false;
    }else if(key == GLFW_KEY_A) {
      a_pressed = false;
    }else if(key == GLFW_KEY_S) {
      s_pressed = false;
    }else if(key == GLFW_KEY_D) {
      d_pressed = false;
    }else if(key == GLFW_KEY_UP) {
      up_key_pressed = false;
    }else if(key == GLFW_KEY_DOWN) {
      down_key_pressed = false;
    }
  }
}

int main() {
  std::shared_ptr<VulkanEngine> engine = setupEngine();

  //setup callbacks
  glfwSetCursorPosCallback(engine->getDisplay()->getInternalWindow(), glfwMouseCallback);
  glfwSetKeyCallback(engine->getDisplay()->getInternalWindow(), glfwKeyCallback);

  Renderer renderer = Renderer(engine);

  std::vector<Vertex> triangle = {
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
  };

  std::vector<Vertex> rectangle = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},   
    
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, 
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  
  };


  std::vector<Vertex> cube = {
    {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1, 0, 0}}, //front
    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0, 1, 0}},
    {{0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1, 1, 0}},

    {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1, 0, 0}},
    {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0, 0, 0}},
    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0, 1, 0}},   

    {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 1, 0}}, //back
    {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},
    {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}},

    {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},  
    {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 0, 0}},
    {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}},

    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0, 0, 0}}, //left
    {{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 0}},
    {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1, 0, 0}},

    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0, 0, 0}},
    {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0, 1, 0}},
    {{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1, 1, 0}},

    {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0, 0, 0}}, //right
    {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0, 1, 0}},
    {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1, 0, 0}},

    {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0, 1, 0}},
    {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1, 1, 0}},
    {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1, 0, 0}},
    
    {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0, 0, 0}}, //top
    {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {0, 1, 0}},
    {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1, 0, 0}},

    {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {0, 1, 0}},
    {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1, 1, 0}},
    {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1, 0, 0}},

    {{0.0f, 0.0f, 0.0f}, {1.0f, 0.5f, 0.5f}, {0, 0, 0}}, //bottom
    {{1.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.5f}, {1, 1, 0}},
    {{1.0f, 0.0f, 0.0f}, {1.0f, 0.5f, 0.5f}, {1, 0, 0}},

    {{0.0f, 0.0f, 0.0f}, {1.0f, 0.5f, 0.5f}, {0, 0, 0}},
    {{0.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.5f}, {0, 1, 0}},
    {{1.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.5f}, {1, 1, 0}},
  };

  std::vector<Vertex> cube2 = cube;

  for(Vertex& v : cube2) {
    v.texCoord[2] = 1;
  }

  std::vector<InstanceData> instanceDataCube {
    
  };

  std::vector<InstanceData> instanceDataCube2 {
    
  };

  for(int x = 0; x < 60; ++x) {
    for(int y = 0; y < 60; ++y) {
      for(int z = 0; z < 60; ++z) {
        instanceDataCube.push_back(InstanceData({{x, y, z}}));
        instanceDataCube2.push_back(InstanceData({{-x, y, z}}));
      }
    }
  }

  renderer.setDataPair("block1", cube, instanceDataCube);
  renderer.setDataPair("block2", cube2, instanceDataCube2);

  while(!engine->getDisplay()->shouldWindowClose()) {
    renderer.getXRotation() += xDelta;
    renderer.getYRotation() -= yDelta;

    if(renderer.getYRotation() > 90) {
      renderer.getYRotation() = 90;
    }else if(renderer.getYRotation() < -90) {
      renderer.getYRotation() = -90;
    }

    xDelta = 0;
    yDelta = 0;

    if(w_pressed) {
      renderer.getCameraPosition() = renderer.getCameraPosition() + glm::vec3(0.01 * sin(glm::radians(renderer.getXRotation())), 0, 0.01 * cos(glm::radians(renderer.getXRotation())));
    }

    if(s_pressed) {
      renderer.getCameraPosition() = renderer.getCameraPosition() + glm::vec3(-0.01 * sin(glm::radians(renderer.getXRotation())), 0, -0.01 * cos(glm::radians(renderer.getXRotation())));
    }

    if(a_pressed) {
      renderer.getCameraPosition() = renderer.getCameraPosition() + glm::vec3(0.01 * cos(glm::radians(renderer.getXRotation())), 0, -0.01 * sin(glm::radians(renderer.getXRotation())));
    }

    if(d_pressed) {
      renderer.getCameraPosition() = renderer.getCameraPosition() + glm::vec3(-0.01 * cos(glm::radians(renderer.getXRotation())), 0, 0.01 * sin(glm::radians(renderer.getXRotation())));
    }

    if(up_key_pressed) {
      renderer.getCameraPosition() = renderer.getCameraPosition() + glm::vec3(0, 0.02, 0);
    }

    if(down_key_pressed) {
      renderer.getCameraPosition() = renderer.getCameraPosition() + glm::vec3(0, -0.02, 0);
    }

    renderer.recordCommandBuffers();

    renderer.renderFrame();

    glfwPollEvents();
  }

  return 0;
}