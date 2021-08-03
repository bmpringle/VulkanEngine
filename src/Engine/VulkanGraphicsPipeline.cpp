#include "VulkanGraphicsPipeline.h"

#include <fstream>

#include "VulkanEngine.h"

VulkanGraphicsPipeline::VulkanGraphicsPipeline() {
    createInfoVertexStage = {};
    createInfoVertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfoVertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    createInfoVertexStage.pName = "main";

    createInfoFragmentStage = {};
    createInfoFragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfoFragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    createInfoFragmentStage.pName = "main";

    vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; 
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; 
    multisampling.pSampleMask = nullptr; 
    multisampling.alphaToCoverageEnable = VK_FALSE; 
    multisampling.alphaToOneEnable = VK_FALSE; 

    colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; 
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; 
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; 
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; 

    colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; 
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; 
    colorBlending.blendConstants[1] = 0.0f; 
    colorBlending.blendConstants[2] = 0.0f; 
    colorBlending.blendConstants[3] = 0.0f; 

    pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = 0; 
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
}

void VulkanGraphicsPipeline::create(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain) {
    std::vector<char> vertShaderCode = readFile(vertexShader);
    std::vector<char> fragShaderCode = readFile(fragmentShader);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, device);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, device);

    createInfoVertexStage.module = vertShaderModule;
    createInfoFragmentStage.module = fragShaderModule;

    VkPipelineShaderStageCreateInfo shaderStages[] = {createInfoVertexStage, createInfoFragmentStage};

    viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapchain->getInternalExtent2D().width;
    viewport.height = (float) swapchain->getInternalExtent2D().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = swapchain->getInternalExtent2D();

    viewportState.pViewports = &viewport;
    viewportState.pScissors = &scissor;

    vertexInputInfo.vertexBindingDescriptionCount = vertexInputBindingDescriptions.size();
    vertexInputInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = vertexInputAttributeDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    layoutBindings.push_back(samplerLayoutBinding);

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(device->getInternalLogicalDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

     layoutBindings.pop_back();

    pipelineLayoutInfo.setLayoutCount = 1; 
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; 

    if(vkCreatePipelineLayout(device->getInternalLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = pipelineLayout;

    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.renderPass = swapchain->getInternalRenderPass();
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(device->getInternalLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapchain->getInternalImages().size());
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(swapchain->getInternalImages().size());

    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapchain->getInternalImages().size());

    if (vkCreateDescriptorPool(device->getInternalLogicalDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    std::vector<VkDescriptorSetLayout> layouts(swapchain->getInternalImages().size(), descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchain->getInternalImages().size());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(swapchain->getInternalImages().size());
    if (vkAllocateDescriptorSets(device->getInternalLogicalDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    vkDestroyShaderModule(device->getInternalLogicalDevice(), vertShaderModule, nullptr);
    vkDestroyShaderModule(device->getInternalLogicalDevice(), fragShaderModule, nullptr);
}

void VulkanGraphicsPipeline::destroyGraphicsPipeline(std::shared_ptr<VulkanDevice> device) {
    vkDestroyDescriptorPool(device->getInternalLogicalDevice(), descriptorPool, nullptr);

    if(isDescriptorLayoutSet) {
        vkDestroyDescriptorSetLayout(device->getInternalLogicalDevice(), descriptorSetLayout, nullptr);
    }
    vkDestroyPipeline(device->getInternalLogicalDevice(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device->getInternalLogicalDevice(), pipelineLayout, nullptr);
}

std::vector<char> VulkanGraphicsPipeline::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule VulkanGraphicsPipeline::createShaderModule(const std::vector<char>& shaderCode, std::shared_ptr<VulkanDevice> device) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
    createInfo.codeSize = shaderCode.size();

    VkShaderModule shader;
    
    if(vkCreateShaderModule(device->getInternalLogicalDevice(), &createInfo, nullptr, &shader) != VK_SUCCESS) {
        std::runtime_error("failed to create shader");
    }

    return shader;
}

void VulkanGraphicsPipeline::setVertexShader(std::string shader) {
    vertexShader = shader;
}

void VulkanGraphicsPipeline::setFragmentShader(std::string shader) {
    fragmentShader = shader;
}

VkPipeline& VulkanGraphicsPipeline::getInternalGraphicsPipeline() {
    return graphicsPipeline;
}

void VulkanGraphicsPipeline::setVertexInputBindingDescriptions(std::vector<VkVertexInputBindingDescription> desc) {
    vertexInputBindingDescriptions = desc;
}

void VulkanGraphicsPipeline::setVertexInputAttributeDescriptions(std::vector<VkVertexInputAttributeDescription> desc) {
    vertexInputAttributeDescriptions = desc;
}

void VulkanGraphicsPipeline::addDescriptorSetLayoutBinding(VkDescriptorSetLayoutBinding binding) {
    layoutBindings.push_back(binding);
    isDescriptorLayoutSet = true;
}

std::vector<VkDescriptorSet>& VulkanGraphicsPipeline::getDescriptorSets() {
    return descriptorSets;
}

VkPipelineLayout& VulkanGraphicsPipeline::getPipelineLayout() {
    return pipelineLayout;
}