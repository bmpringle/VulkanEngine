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

        VkDescriptorSetLayoutBinding& getDescriptorSetLayoutBinding(int index);

        void setDescriptorPoolData(VkDescriptorType type, uint32_t size);

        void setPushConstantDescriptor(VkPushConstantRange desc);

        void setDescriptorSetLayoutFlags(VkDescriptorSetLayoutCreateFlags flags);

        void setDescriptorPoolFlags(VkDescriptorPoolCreateFlags flags);

        void setPipelineCreateInfoFlags(VkPipelineCreateFlags flags);

        void setPipelineBase(VkPipeline base);

        void setCanHaveDerivatives(bool canBeParent);

        void setPolygonType(VkPolygonMode mode);

        //this function is ignored, because the wideLines device feature is so rare. As such we don't enable it, and all lines are 1.0f
        void setLineWidth(float width);

        void setPrimitiveTopology(VkPrimitiveTopology top);
    private:
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

        std::vector<std::pair<VkDescriptorType, uint32_t>> poolData;

        VkDescriptorPoolCreateInfo poolInfo;

        std::vector<VkPushConstantRange> pushContsantDescriptors;

        VkDescriptorSetLayoutCreateFlags descriptorSetLayoutFlags = 0;

        VkDescriptorPoolCreateFlags descriptorPoolFlags = 0;

        VkPipelineCreateFlags pipelineCreateFlags = 0;

        VkPipeline basePipeline = VK_NULL_HANDLE;

        bool canHaveDerivatives = false;

        VkPolygonMode polygonType = VK_POLYGON_MODE_FILL;

        float lineWidth = 1.0;

        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
};

#endif