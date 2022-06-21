#ifndef VULKANDISPLAY_H
#define VULKANDISPLAY_H

#include "VulkanInclude.h"
#include "VulkanInstance.h"
#include <string>
#include <functional>

#include <memory>

class VulkanDisplay {
    public:
        VulkanDisplay();

        void destroyDisplay(std::shared_ptr<VulkanInstance> instance);

        void create(std::shared_ptr<VulkanInstance> instance);

        void setWindowName(std::string newWindowName);

        bool isCreated();

        void setInitialWindowDimensions(unsigned int width, unsigned int height);

        bool shouldWindowClose();

        GLFWwindow* getInternalWindow();

        VkSurfaceKHR& getInternalSurface();

        void setFramebufferResized(bool resized);

        bool getFramebufferResized();

        void setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> callback);

        void setCursorPosCallback(std::function<void(GLFWwindow*, double, double)> callback);

        void setMouseButtonCallback(std::function<void(GLFWwindow*, int, int, int)> callback);

        void setScrollCallback(std::function<void(GLFWwindow*, double, double)> callback);

        void handleKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

        void handleCursorPosCallback(GLFWwindow* window, double x, double y);

        void handleMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

        void handleScrollCallback(GLFWwindow* window, double x, double y);

    private:
        GLFWwindow* window;
        VkSurfaceKHR surface;

        bool hasBeenCreated = false;

        std::string windowName = "Vulkan Window";

        GLFWmonitor* monitor = nullptr;

        unsigned int width = 800;
        unsigned int height = 600;

        bool framebufferResized = false;

        std::function<void(GLFWwindow*, int, int, int, int)> keyCallbackFunc = [](GLFWwindow* window, int key, int scancode, int action, int mods) {

        };

        std::function<void(GLFWwindow*, double, double)> cursorPosCallbackFunc = [](GLFWwindow* window, double x, double y) {

        };

        std::function<void(GLFWwindow*, int, int, int)> mouseButtonCallbackFunc = [](GLFWwindow* window, int button, int action, int mods) {

        };

        std::function<void(GLFWwindow*, double, double)> scrollCallbackFunc = [](GLFWwindow* window, double x, double y) {

        };
};

#endif