/* note: this was created in order to learn vulkan, the tutorial followed was https://vulkan-tutorial.com/
 * as such, while many things will be changed as I write this program, some parts may be very similar or identical.
*/

#include "VKRenderer.h"

#include <iostream>

#include <chrono>

#include <glm/gtx/string_cast.hpp>

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
  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1, 0, 0}}, //front
  {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},

  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1, 0, 0}},
  {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}},
  {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0, 1, 0}},   

  {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 1, 0}}, //back
  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},
  {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}},

  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},  
  {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 0, 0}},
  {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}},

  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}}, //left
  {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},
  {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 0, 0}},

  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}},
  {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},

  {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}}, //right
  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1, 0, 0}},

  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},
  {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1, 0, 0}},
  
  {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}}, //top
  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1, 0, 0}},

  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},
  {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1, 0, 0}},

  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}}, //bottom
  {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},
  {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1, 0, 0}},

  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0, 0, 0}},
  {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 0}},
};

static std::vector<TransparentVertex> transparent_cube = {
  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 0, 0}}, //front
  {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 1, 0}},

  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 0, 0}},
  {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0}},
  {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 1, 0}},   

  {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 1, 0}}, //back
  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 1, 0}},
  {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0}},

  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 1, 0}},  
  {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 0, 0}},
  {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0}},

  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0}}, //left
  {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 1, 0}},
  {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 0, 0}},

  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0}},
  {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 1, 0}},

  {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0}}, //right
  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 0, 0}},

  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 1, 0}},
  {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 0, 0}},
  
  {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0}}, //top
  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 0, 0}},

  {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 1, 0}},
  {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 0, 0}},

  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0}}, //bottom
  {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 1, 0}},
  {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 0, 0}},

  {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0}},
  {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 1, 0}},
  {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 1, 0}},
};

static std::vector<OverlayVertex> texturedRectangleOverlay {
  {{0, 0, 0}, {1, 1, 1}, {0, 0}, 0},
  {{75, 75, 0}, {1, 1, 1}, {1, 1}, 0},
  {{75, 0, 0}, {1, 1, 1}, {1, 0}, 0},

  {{0, 0, 0}, {1, 1, 1}, {0, 0}, 0},
  {{0, 75, 0}, {1, 1, 1}, {0, 1}, 0},
  {{75, 75, 0}, {1, 1, 1}, {1, 1}, 0},
};

static std::vector<OverlayVertex> untexturedRectangleOverlay {
  {{0, -100, 0}, {1, 1, 0}, {0, 0}, 0},
  {{75, -25, 0}, {1, 1, 0}, {0, 0}, 0},
  {{75, -100, 0}, {1, 1, 0}, {0, 0}, 0},

  {{0, -100, 0}, {1, 1, 0}, {0, 0}, 0},
  {{0, -25, 0}, {1, 1, 0}, {0, 0}, 0},
  {{75, -25, 0}, {1, 1, 0}, {0, 0}, 0},
};

float testChangingOverlayX = 100;
float testChangingOverlayY = 100;

glm::vec4 testChangingClearColor = glm::vec4(0, 0, 0, 1);

int main() {
  //define variables
  double previousMouseX = -1; //-1 means it hasn't been set yet
  double previousMouseY = -1; //-1 means it hasn't been set yet

  float sensitivity = 0.1;

  float xDelta = 0;
  float yDelta = 0;

  bool w_pressed = false;
  bool a_pressed = false;
  bool s_pressed = false;
  bool d_pressed = false;
  bool up_key_pressed = false;
  bool down_key_pressed = false;
  bool g_key_pressed = false;
  bool l_key_pressed = false;
  bool z_key_pressed = false;
  bool esc_key_pressed = false;

  bool flag = false;
  bool flag1 = false;

  //init renderer
  VKRenderer renderer = VKRenderer();

  //setup input callbacks
  std::function<void(GLFWwindow*, double, double)> mouseCallback = [&](GLFWwindow* window, double mouseX, double mouseY) {
    if(previousMouseX != -1) {
      xDelta = (previousMouseX - mouseX) * sensitivity;
    }
    previousMouseX = mouseX;

    if(previousMouseY != -1) {
      yDelta = (previousMouseY - mouseY) * sensitivity;
    }
    previousMouseY = mouseY;
  };

  renderer.getEngine()->getDisplay()->setCursorPosCallback(mouseCallback);

  std::function<void(GLFWwindow*, int, int, int, int)> keyCallback = [&](GLFWwindow* window, int key, int scancode, int action, int mods) {
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
      }else if(key == GLFW_KEY_L) {
        l_key_pressed = true;
      }else if(key == GLFW_KEY_Z) {
        z_key_pressed = true;
      }else if(key == GLFW_KEY_ESCAPE) {
        esc_key_pressed = true;
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
      }else if(key == GLFW_KEY_L) {
        l_key_pressed = false;
      }else if(key == GLFW_KEY_Z) {
        z_key_pressed = false;
      }else if(key == GLFW_KEY_ESCAPE) {
        esc_key_pressed = false;
      }
    }
  };

  renderer.getEngine()->getDisplay()->setKeyCallback(keyCallback);

  //setup rendering

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

  renderer.loadTextureArray("game-textures", {"assets/dirt.png", "assets/grass_side.png", "assets/glass-new.png", "assets/water.png"});
  renderer.setCurrentTextureArray("game-textures");

  std::vector<Vertex> cube2 = cube;

  for(Vertex& v : cube2) {
    v.texCoord[2] = renderer.getTextureArrayID("game-textures", "assets/grass_side.png");
  }

  for(Vertex& v : cube) {
    v.texCoord[2] = renderer.getTextureArrayID("game-textures", "assets/dirt.png");
  }

  std::vector<InstanceData> instanceDataCube;

  std::vector<InstanceData> instanceDataCube2;

  std::vector<InstanceData> instanceDataCube3;

  std::vector<InstanceData> instanceDataCube4;

  std::vector<InstanceData> instanceDataCube5;

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
        instanceDataCube3.push_back(InstanceData({{-x, y, -z}}));
        instanceDataCube4.push_back(InstanceData({{-x, y, -z*2}}));
        instanceDataCube5.push_back(InstanceData({{x, y, -z*2}}));
      }
    }
  }

  renderer.setModel("block1", cube);
  renderer.setModel("block2", cube2);

  tex_id = renderer.getTextureArrayID("game-textures", "assets/glass-new.png");

  for(TransparentVertex& v : transparent_cube) {
    v.texCoord[2] = tex_id;
  }

  renderer.setModel("translucentcube1", {/*opaque vertices*/}, transparent_cube);

  tex_id = renderer.getTextureArrayID("game-textures", "assets/water.png");

  for(TransparentVertex& v : transparent_cube) {
    v.texCoord[2] = tex_id;
  }

  renderer.setModel("translucentcube2", {/*opaque vertices*/}, transparent_cube);

  std::vector<InstanceData> translucentData = {
    InstanceData({{-1, 0, -2}}), InstanceData({{-1, 0, -3}}), InstanceData({{-1, 0, -4}}), InstanceData({{-1, 0, -5}}),
    InstanceData({{-2, 0, -2}}), InstanceData({{-2, 0, -3}}), InstanceData({{-2, 0, -4}}), InstanceData({{-2, 0, -5}}),
    InstanceData({{-3, 0, -2}}), InstanceData({{-3, 0, -3}}), InstanceData({{-3, 0, -4}}), InstanceData({{-3, 0, -5}})
  };

  std::vector<InstanceData> translucentData2;

  for(int i = 0; i < 10; ++i) {
    for(int j = 0; j < 10; ++j) {
      for(int k = 0; k < 10; ++k) {
        translucentData2.push_back(InstanceData({{-10 - i, 5 - j, -10 - k}}));
      }
    }
  }

  renderer.addInstancesToModel("translucentcube1", "set1", translucentData);

  renderer.addInstancesToModel("translucentcube2", "set1", translucentData2);

  renderer.addInstancesToModel("block1", "set1", instanceDataCube);

  renderer.addInstancesToModel("block2", "set1", instanceDataCube2);

  

  renderer.setOverlayVertices("exampleRectOverlay", texturedRectangleOverlay);
  renderer.setOverlayVertices("textOverlay", texturedRectangleOverlay2);

  for(OverlayVertex& v : untexturedRectangleOverlay) {
    v.texID = renderer.getTextureID("UNTEXTURED");
  }

  renderer.setOverlayVertices("exampleUntexturedRectOverlay", untexturedRectangleOverlay);

  std::vector<OverlayVertex> texturedRectangleOverlay3 = texturedRectangleOverlay2;

  tex_id = renderer.getTextureID("text1");

  for(OverlayVertex& v : texturedRectangleOverlay3) {
    v.position.x -= 20;
    v.position.z = 99;
    v.texID = tex_id;
  }

  renderer.setOverlayVertices("textOverlay2", texturedRectangleOverlay3);

  std::vector<InstanceData> instanceDataCube6;

  instanceDataCube6.push_back(InstanceData({{-3, 0, -3}}));
  instanceDataCube6.push_back(InstanceData({{-1, 0, -1}}));

  renderer.setWireframeTopology(VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);

  std::vector<WireframeVertex> wireframe = {
    {{0.0f, 0.0f, 0.0f}},
    {{0.0f, 1.0f, 0.0f}},
    {{1.0f, 1.0f, 0.0f}},
    {{1.0f, 0.0f, 0.0f}},
    {{0.0f, 0.0f, 0.0f}},

    {{0.0f, 0.0f, 1.0f}},
    {{0.0f, 1.0f, 1.0f}},
    {{1.0f, 1.0f, 1.0f}},
    {{1.0f, 0.0f, 1.0f}},
    {{0.0f, 0.0f, 1.0f}},

    {{1.0f, 0.0f, 1.0f}},
    {{1.0f, 0.0f, 0.0f}},

    {{1.0f, 1.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f}},

    {{0.0f, 1.0f, 1.0f}},
    {{0.0f, 1.0f, 0.0f}},
  };

  renderer.setWireframeModel("wireframe1", wireframe);

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

      renderer.addTextTexture("text1", "this is text in my\nvulkan engine\nafter changing the texture!");
    }

    if(l_key_pressed) {
      renderer.addInstancesToModel("block2", "set1", instanceDataCube3);

      renderer.addInstancesToModel("block2", "set2", instanceDataCube4);

      renderer.setScreenTint(glm::vec3(1, 0, 1));
    }

    if(z_key_pressed && !flag1) {
      if(!flag) {
        renderer.addInstancesToModel("block1", "set1", instanceDataCube);
        renderer.addInstancesToWireframeModel("wireframe1", "set1", instanceDataCube6);
      }else {
        renderer.removeInstancesFromModel("block1", "set1");
        renderer.removeInstancesFromWireframeModel("wireframe1", "set1");
      }
      flag = !flag;
      flag1 = true;
    }else if(!z_key_pressed) {
      flag1 = false;
    }

    if(esc_key_pressed) {
      glfwSetWindowShouldClose(renderer.getEngine()->getDisplay()->getInternalWindow(), true);
    }

    renderer.recordCommandBuffers();

    renderer.renderFrame(); 

    glfwPollEvents();
  }
  return 0;
}