#include "VulkanInstance.h"

#include <iostream>

#ifdef __APPLE__
#include <MoltenVK/vk_mvk_moltenvk.h>
#endif

VulkanInstance::VulkanInstance() {

}

void VulkanInstance::addValidationLayer(const char* layer) {
    validationLayers.push_back(layer);
}

std::vector<VkExtensionProperties> VulkanInstance::getAllSupportedExtensions() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions = std::vector<VkExtensionProperties>(extensionCount);

  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  return extensions;
}

void VulkanInstance::tryAddInstanceExtension(const char* extension) {
    std::vector<VkExtensionProperties> extensions = getAllSupportedExtensions();

    bool hasExtension = false;

    for(VkExtensionProperties& extensionProperty : extensions) {
        if(extensionProperty.extensionName == std::string(extension)) {
            hasExtension = true;
        }
    }

    if(hasExtension) {
        instanceExtensions.push_back(extension);
    }else {
        throw std::runtime_error(std::string(extension) + " isn't supported on this vulkan implementation");
    }
}

bool VulkanInstance::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : validationLayers) {
        bool layerFound = false;

        for(const auto& layerProperties : availableLayers) {
            if(std::string(layerName) == std::string(layerProperties.layerName)) {
                layerFound = true;
                break;
            }
        }

        if(!layerFound) {
            return false;
        }
    }

    return true;
}

void VulkanInstance::create() {
    #ifdef __APPLE__
    MVKConfiguration config {};
    size_t configSize = sizeof(MVKConfiguration);
    vkGetMoltenVKConfigurationMVK(VK_NULL_HANDLE, &config, &configSize); //set beta argument buffers to be used in order to get more descriptors than 16 for frag shader.
    
    config.useMetalArgumentBuffers = MVK_CONFIG_USE_METAL_ARGUMENT_BUFFERS_DESCRIPTOR_INDEXING;

    configSize = sizeof(MVKConfiguration);
    vkSetMoltenVKConfigurationMVK(VK_NULL_HANDLE, &config, &configSize);

    #endif

    std::vector<VkExtensionProperties> extensions = getAllSupportedExtensions();

    if(!checkValidationLayerSupport()) {
        throw std::runtime_error("some of the validation layers you requested couldn't be found!");
    }

    //Create VkApplicationInfo struct and fill with data
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName.data();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "A Bad Engine (TM) (Patent Pending)";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    //Create VkInstanceCreateInfo struct and fill with data
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    //get extensions required by glfw to render with vulkan
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    if(glfwExtensionCount == 0) {
        throw std::runtime_error("an error occured while getting instance extensions from glfw");
    }

    std::vector<const char*> extensionsVector = instanceExtensions;
    
    for(size_t i = 0; i < glfwExtensionCount; ++i) {
        extensionsVector.push_back(glfwExtensions[i]);
    }

    #ifdef __APPLE__ 
    extensionsVector.push_back("VK_KHR_get_physical_device_properties2"); //required for MoltenVK
    extensionsVector.push_back("VK_KHR_portability_enumeration"); //required for MoltenVK
    #endif

    createInfo.enabledExtensionCount = extensionsVector.size();
    createInfo.ppEnabledExtensionNames = extensionsVector.data();

    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
    createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    //attempt to create VkInstance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("couldn't create vkinstance: " + std::to_string(result));
    }else {
        std::cout << "successfully created VkInstance!" << std::endl;
    }

    hasBeenCreated = true;
}

void VulkanInstance::setAppName(std::string newName) {
    appName = newName;
}

VkInstance& VulkanInstance::getInternalInstance() {
    return instance;
}

bool VulkanInstance::isCreated() {
    return hasBeenCreated;
}

void VulkanInstance::destroyInstance() {
    vkDestroyInstance(instance, nullptr);
}

std::vector<const char*> VulkanInstance::getValidationLayers() {
    return validationLayers;
}