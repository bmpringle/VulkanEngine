#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#include "VulkanInclude.h"
#include "VulkanSwapchain.h"

class VulkanGraphicsPipeline {
    public:
        VulkanGraphicsPipeline();

        void create(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain);

        void destroyGraphicsPipeline(std::shared_ptr<VulkanDevice> device);

        void setVertexShader(std::string shader);

        void setFragmentShader(std::string shader);

        VkPipeline& getInternalGraphicsPipeline();
    
    private:
        std::vector<char> readFile(const std::string& filename);

        VkShaderModule createShaderModule(const std::vector<char>& shaderCode, std::shared_ptr<VulkanDevice> device);

        VkPipeline graphicsPipeline;

        std::string vertexShader = "shaders/output/vert.spv";
        std::string fragmentShader = "shaders/output/frag.spv";

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
};

#endif