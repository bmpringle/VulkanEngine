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

static void cursorPosCallback(GLFWwindow* window, double x, double y) {
    auto display = reinterpret_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
    display->handleCursorPosCallback(window, x, y);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto display = reinterpret_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
    display->handleKeyCallback(window, key, scancode, action, mods);
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto display = reinterpret_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
    display->handleMouseButtonCallback(window, button, action, mods);
}

static void scrollCallback(GLFWwindow* window, double x, double y) {
    auto display = reinterpret_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
    display->handleScrollCallback(window, x, y);
}

void VulkanDisplay::create(std::shared_ptr<VulkanInstance> instance) {
    window = glfwCreateWindow(width, height, windowName.data(), monitor, nullptr);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

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

void VulkanDisplay::setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> callback) {
    keyCallbackFunc = callback;
}

void VulkanDisplay::setCursorPosCallback(std::function<void(GLFWwindow*, double, double)> callback) {
    cursorPosCallbackFunc = callback;
}

void VulkanDisplay::setMouseButtonCallback(std::function<void(GLFWwindow*, int, int, int)> callback) {
    mouseButtonCallbackFunc = callback;
}

void VulkanDisplay::setScrollCallback(std::function<void(GLFWwindow*, double, double)> callback) {
    scrollCallbackFunc = callback;
}

void VulkanDisplay::handleKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    keyCallbackFunc(window, key, scancode, action, mods);
}

void VulkanDisplay::handleCursorPosCallback(GLFWwindow* window, double x, double y) {
    cursorPosCallbackFunc(window, x, y);
}

void VulkanDisplay::handleMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    mouseButtonCallbackFunc(window, button, action, mods);
}

void VulkanDisplay::handleScrollCallback(GLFWwindow* window, double x, double y) {
    scrollCallbackFunc(window, x, y);
}