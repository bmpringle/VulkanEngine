#include "VulkanDevice.h"

VulkanDevice::VulkanDevice() : deviceExtensions({
    "VK_KHR_swapchain",
    #ifdef __APPLE__
    "VK_KHR_portability_subset" //required for MoltenVK
    #endif
}){

}

void VulkanDevice::destroyDevice() {
    vkDestroyCommandPool(logicalDevice, graphicsCommandPool, nullptr);

    vkDestroyDevice(logicalDevice, nullptr);
}

void VulkanDevice::create(VulkanInstance& instance, VulkanDisplay& display) {
    createPhysicalDevice(instance, display);
    createLogicalDevice(instance);
    createCommandPool();

    hasBeenCreated = true;
}

void VulkanDevice::createPhysicalDevice(VulkanInstance& instance, VulkanDisplay& display) {
    //pick VkPhysicalDevice
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.getInternalInstance(), &deviceCount, nullptr);

    if(deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    //check if there is a device that has the requirements for the application met
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance.getInternalInstance(), &deviceCount, devices.data());

    for(const VkPhysicalDevice& device : devices) {
        if(canDeviceBeUsed(device, display)) {
            physicalDevice = device;
        }
    }

    if(physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("No useable GPU!");
    }

    //find the indices of the queue families the device supports
    indices = getDeviceQueueFamilies(physicalDevice, display);
}

void VulkanDevice::createLogicalDevice(VulkanInstance& instance) {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = std::vector<VkDeviceQueueCreateInfo>();

    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for(uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    //no longer used
    createInfo.enabledLayerCount = 0;

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}

QueueFamilyIndices VulkanDevice::getDeviceQueueFamilies(VkPhysicalDevice pDevice, VulkanDisplay& display) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for(const auto& queueFamily : queueFamilies) {
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, display.getInternalSurface(), &presentSupport);

        if(presentSupport) {
            indices.presentFamily = i;
        }

        if(indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

bool VulkanDevice::canDeviceBeUsed(VkPhysicalDevice device, VulkanDisplay& display) {
    QueueFamilyIndices indices = getDeviceQueueFamilies(device, display);

    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool validSwapChain = false;

    if(extensionsSupported) { //only query for swap-chain details once we know that the extension is supported
        SwapChainSupportDetails details = getDeviceSwapChainSupport(device, display);
        validSwapChain = details.formats.size() != 0 && details.presentModes.size() != 0;
    }

    return indices.isComplete() && extensionsSupported && validSwapChain;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for(const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanDevice::getDeviceSwapChainSupport(VkPhysicalDevice device, VulkanDisplay& display) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, display.getInternalSurface(), &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, display.getInternalSurface(), &formatCount, nullptr);

    if(formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, display.getInternalSurface(), &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, display.getInternalSurface(), &presentModeCount, nullptr);

    if(presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, display.getInternalSurface(), &presentModeCount, details.presentModes.data());
    }

    return details;
}

void VulkanDevice::addDeviceExtension(const char* deviceExtension) {
    deviceExtensions.push_back(deviceExtension);
}

bool VulkanDevice::isCreated() {
    return hasBeenCreated;
}

SwapChainSupportDetails VulkanDevice::getDeviceSwapChainSupport(VulkanDisplay& display) {
    return getDeviceSwapChainSupport(physicalDevice, display);
}

VkDevice& VulkanDevice::getInternalLogicalDevice() {
    return logicalDevice;
}

VkPhysicalDevice& VulkanDevice::getInternalPhysicalDevice() {
    return physicalDevice;
}

QueueFamilyIndices VulkanDevice::getDeviceQueueFamilies(VulkanDisplay& display) {
    return VulkanDevice::getDeviceQueueFamilies(physicalDevice, display);
}

void VulkanDevice::createCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if(vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &graphicsCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool");
    }
}

VkCommandPool& VulkanDevice::getInternalCommandPool() {
    return graphicsCommandPool;
}

VkQueue& VulkanDevice::getInternalGraphicsQueue() {
    return graphicsQueue;
}

VkQueue& VulkanDevice::getInternalPresentQueue() {
    return presentQueue;
}