#include "VulkanDisplay.h"

VulkanDisplay::VulkanDisplay() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void VulkanDisplay::setWindowName(std::string newWindowName) {
    windowName = newWindowName;
}

void VulkanDisplay::destroyDisplay(std::shared_ptr<VulkanInstance> instance) {
    glfwDestroyWindow(window);

    glfwTerminate();

    vkDestroySurfaceKHR(instance->getInternalInstance(), surface, nullptr);
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto display = reinterpret_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
    display->setFramebufferResized(true);
}

void VulkanDisplay::create(std::shared_ptr<VulkanInstance> instance) {
    window = glfwCreateWindow(width, height, windowName.data(), monitor, nullptr);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    if(glfwCreateWindowSurface(instance->getInternalInstance(), window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

bool VulkanDisplay::isCreated() {
    return hasBeenCreated;
}

void VulkanDisplay::setInitialWindowDimensions(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
}

bool VulkanDisplay::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}

GLFWwindow* VulkanDisplay::getInternalWindow() {
    return window;
}

VkSurfaceKHR& VulkanDisplay::getInternalSurface() {
    return surface;
}

void VulkanDisplay::setFramebufferResized(bool resized) {
    framebufferResized = resized;
}

bool VulkanDisplay::getFramebufferResized() {
    return framebufferResized;
}