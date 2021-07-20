#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include "VulkanInclude.h"

#include <vector>
#include <string>

class VulkanInstance {

    public:
        //get all supported instance extensions
        std::vector<VkExtensionProperties> getAllSupportedExtensions();

        void setAppName(std::string newName);

        //add a validation layer
        void addValidationLayer(const char* layer);

        //will throw an error if the extension isn't supported. Note: you DO NOT have to add extensions that glfw already needs to work, those will be automatically requested
        void tryAddInstanceExtension(const char* extension);

        //generate instance with the settings that have been set. VulkanEngine will call this automatically so you don't need to.
        void create();

        /*should only be used internally by the engine, not reccomended to be used otherwise. however, if you REALLY want to do something that the 
        engine doesn't support, you can get the VkInstance.
        */
        VkInstance getInternalInstance();

        //check if VulkanInstance::create() has been called
        bool isCreated();

        //destroy instance
        void destroyInstance();

        std::vector<const char*> getValidationLayers();

    private:
        bool checkValidationLayerSupport();

        VkInstance instance;

        std::string appName; //this is just for vulkan app info, has nothing to do with your window.

        std::vector<const char*> validationLayers;

        std::vector<const char*> instanceExtensions;

        bool hasBeenCreated = false;
};

#endif