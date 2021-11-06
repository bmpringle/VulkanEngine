/* note: this was created in order to learn vulkan, the tutorial followed was https://vulkan-tutorial.com/
 * as such, while many things will be changed as I write this program, some parts may be very similar or identical.
*/

#include "VKRenderer.h"

#include <iostream>

#include <chrono>

#include <glm/gtx/string_cast.hpp>

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
bool g_key_pressed = false;

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
    }else if(key == GLFW_KEY_G) {
      g_key_pressed = true;
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
    }else if(key == GLFW_KEY_G) {
      g_key_pressed = false;
    }
  }
}

static std::vector<Vertex> triangle = {
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
  };

static std::vector<Vertex> rectangle = {
  {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
  {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
  {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},   
  
  {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
  {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, 
  {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  
};


static std::vector<Vertex> cube = {
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

static std::vector<OverlayVertex> texturedRectangleOverlay {
  {{0, 0, 0}, {0, 1, 0}, {0, 0}, 0},
  {{75, 75, 0}, {0, 1, 0}, {1, 1}, 0},
  {{75, 0, 0}, {0, 1, 0}, {1, 0}, 0},

  {{0, 0, 0}, {0, 1, 0}, {0, 0}, 0},
  {{0, 75, 0}, {0, 1, 0}, {0, 1}, 0},
  {{75, 75, 0}, {0, 1, 0}, {1, 1}, 0},
};

float testChangingOverlayX = 100;
float testChangingOverlayY = 100;

glm::vec4 testChangingClearColor = glm::vec4(0, 0, 0, 1);

int main() {
  VKRenderer renderer = VKRenderer();

  renderer.addTexture("test", "assets/test.jpg");
  renderer.addTexture("cube-cube", "assets/cube-cube.png");
  renderer.addTexture("dirt", "assets/dirt.png");
  renderer.addTexture("old-test", "assets/test-OLD.jpg");
  renderer.addTexture("grass-side", "assets/grass_side.png");
  renderer.addTextTexture("text1", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text2", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text3", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text4", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text5", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text6", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text7", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text8", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text9", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text10", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text11", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text12", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text13", "this is text in my\nvulkan engine!");
  renderer.addTextTexture("text14", "this is text in my\nvulkan engine!\nafter lots of descriptors!");

  renderer.loadTextureArray("game-textures", {"assets/dirt.png", "assets/grass_side.png"});
  renderer.setCurrentTextureArray("game-textures");

  //setup callbacks
  glfwSetCursorPosCallback(renderer.getEngine()->getDisplay()->getInternalWindow(), glfwMouseCallback);
  glfwSetKeyCallback(renderer.getEngine()->getDisplay()->getInternalWindow(), glfwKeyCallback);

  std::vector<Vertex> cube2 = cube;

  for(Vertex& v : cube2) {
    v.texCoord[2] = 2;
  }

  std::vector<InstanceData> instanceDataCube;

  std::vector<InstanceData> instanceDataCube2;

  unsigned int tex_id = renderer.getTextureID("test");

  for(OverlayVertex& v : texturedRectangleOverlay) {
    v.texID = tex_id;
  }

  std::vector<OverlayVertex> texturedRectangleOverlay2 = texturedRectangleOverlay;

  tex_id = renderer.getTextureID("text14");

  auto dimensions = renderer.getTextureDimensions("text14");

  float textureAspectRatio = (float) dimensions.second / (float) dimensions.first;

  float yCoord = 75 * textureAspectRatio;

  for(OverlayVertex& v : texturedRectangleOverlay2) {
    if(v.position[1] == 75) {
      v.position[1] = yCoord;
    }

    v.position[0] = v.position[0] - 75;

    v.texID = tex_id;
  }

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

  renderer.setOverlayVertices("exampleRectOverlay", texturedRectangleOverlay);
  renderer.setOverlayVertices("textOverlay", texturedRectangleOverlay2);

  std::vector<OverlayVertex> texturedRectangleOverlay3 = texturedRectangleOverlay2;

  tex_id = renderer.getTextureID("text1");

  for(OverlayVertex& v : texturedRectangleOverlay3) {
    v.position.x -= 20;
    v.position.z = 99;
    v.texID = tex_id;
  }

  renderer.setOverlayVertices("textOverlay2", texturedRectangleOverlay3);

  while(!renderer.getEngine()->getDisplay()->shouldWindowClose()) {
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

    if(g_key_pressed) {
      testChangingOverlayX = testChangingOverlayX - 0.1;
      testChangingOverlayY = testChangingOverlayY - 0.1;

      renderer.setOverlayBounds(testChangingOverlayX, testChangingOverlayY, 100);

      if(testChangingClearColor.x < 1) {
        testChangingClearColor.x = testChangingClearColor.x + 0.1;
      }else if(testChangingClearColor.y < 1) {
        testChangingClearColor.y = testChangingClearColor.y + 0.1;
      }else if(testChangingClearColor.z < 1) {
        testChangingClearColor.z = testChangingClearColor.z + 0.1;
      }
      renderer.setClearColor(testChangingClearColor);

      renderer.removeOverlayVertices("textOverlay");
    }

    renderer.recordCommandBuffers();

    renderer.renderFrame(); 

    glfwPollEvents();
  }
  return 0;
}