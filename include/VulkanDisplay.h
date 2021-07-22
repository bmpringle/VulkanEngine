#ifndef VULKANDISPLAY_H
#define VULKANDISPLAY_H

#include "VulkanInclude.h"
#include "VulkanInstance.h"
#include <string>

class VulkanDisplay {
    public:
        VulkanDisplay();

        void destroyDisplay(VulkanInstance& instance);

        void create(VulkanInstance& instance);

        void setWindowName(std::string newWindowName);

        bool isCreated();

        void setInitialWindowDimensions(unsigned int width, unsigned int height);

        bool shouldWindowClose();

        GLFWwindow* getInternalWindow();

        VkSurfaceKHR& getInternalSurface();

        void setFramebufferResized(bool resized);

        bool getFramebufferResized();

    private:
        GLFWwindow* window;
        VkSurfaceKHR surface;

        bool hasBeenCreated = false;

        std::string windowName = "Vulkan Window";

        GLFWmonitor* monitor = nullptr;

        unsigned int width = 800;
        unsigned int height = 600;

        bool framebufferResized = false;
};

#endif