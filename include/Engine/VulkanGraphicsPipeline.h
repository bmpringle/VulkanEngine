#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#include "VulkanInclude.h"
#include "VulkanSwapchain.h"

#include "TextureLoader.h"

#include <map>
#include <array>

class VulkanGraphicsPipeline {
    public:
        VulkanGraphicsPipeline();

        void create(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain);

        void destroyGraphicsPipeline(std::shared_ptr<VulkanDevice> device);

        void setVertexShader(std::string shader);

        void setFragmentShader(std::string shader);

        VkPipeline& getInternalGraphicsPipeline();

        std::vector<VkDescriptorSet>& getDescriptorSets();

        VkPipelineLayout& getPipelineLayout();

        void setVertexInputBindingDescriptions(std::vector<VkVertexInputBindingDescription> desc);

        void setVertexInputAttributeDescriptions(std::vector<VkVertexInputAttributeDescription> desc);

        void addDescriptorSetLayoutBinding(VkDescriptorSetLayoutBinding binding);

        void addTextureToLoad(std::string texturePath);

        VkImageView getImageView(std::string texturePath);

        VkSampler getTextureSampler();
    
    private:
        void createTextureImages(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain);

        void createTextureImageViews(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain);

        void createTextureSampler(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain);

        std::vector<char> readFile(const std::string& filename);

        VkShaderModule createShaderModule(const std::vector<char>& shaderCode, std::shared_ptr<VulkanDevice> device);

        VkPipeline graphicsPipeline;

        std::string vertexShader = "shaders/output/vert.spv";
        std::string fragmentShader = "shaders/output/frag.spv";

        std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions = std::vector<VkVertexInputBindingDescription>();

        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions = std::vector<VkVertexInputAttributeDescription>();

        VkPipelineShaderStageCreateInfo createInfoVertexStage;

        VkPipelineShaderStageCreateInfo createInfoFragmentStage;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly;

        VkPipelineViewportStateCreateInfo viewportState;

        VkPipelineRasterizationStateCreateInfo rasterizer;

        VkPipelineMultisampleStateCreateInfo multisampling;

        VkPipelineColorBlendAttachmentState colorBlendAttachment;

        VkPipelineColorBlendStateCreateInfo colorBlending;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo;

        VkGraphicsPipelineCreateInfo pipelineInfo;

        VkViewport viewport;

        VkRect2D scissor;

        VkPipelineLayout pipelineLayout;

        VkDescriptorSetLayout descriptorSetLayout;

        VkDescriptorPool descriptorPool;

        std::vector<VkDescriptorSet> descriptorSets;

        bool isDescriptorLayoutSet = false;

        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

        VkDescriptorPoolCreateInfo poolInfo;

        std::vector<std::string> texturesToLoad;

        TextureLoader textureLoader = TextureLoader();

        std::map<std::string, VkImage> texturePathToImage = std::map<std::string, VkImage>();
        std::map<std::string, VkDeviceMemory> texturePathToDeviceMemory = std::map<std::string, VkDeviceMemory>();
        std::map<std::string, VkImageView> texturePathToImageView = std::map<std::string, VkImageView>();

        VkSampler textureSampler;
};

#endif