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
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
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

    VkPipelineColorBlendAttachmentState colorBlendAttachment;

    colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; 
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; 
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; 
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; 

    colorBlendAttachments.push_back(colorBlendAttachment);

    colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; 
    colorBlending.blendConstants[0] = blendConstants[0]; 
    colorBlending.blendConstants[1] = blendConstants[1]; 
    colorBlending.blendConstants[2] = blendConstants[2]; 
    colorBlending.blendConstants[3] = blendConstants[3]; 

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

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data();
    layoutInfo.flags = descriptorSetLayoutFlags;

    if (vkCreateDescriptorSetLayout(device->getInternalLogicalDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    pipelineLayoutInfo.setLayoutCount = 1; 
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; 

    if(pushContsantDescriptors.size() > 0) {
        pipelineLayoutInfo.pushConstantRangeCount = pushContsantDescriptors.size();
        pipelineLayoutInfo.pPushConstantRanges = pushContsantDescriptors.data();
    }

    if(vkCreatePipelineLayout(device->getInternalLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = depthTesting;
    depthStencil.depthWriteEnable = depthWriting;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    rasterizer.lineWidth = 1.0f;//lineWidth;   
    rasterizer.polygonMode = polygonType;
    rasterizer.cullMode = cullMode;

    inputAssembly.topology = topology;

    colorBlending.attachmentCount = colorBlendAttachments.size();
    colorBlending.pAttachments = colorBlendAttachments.data();

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
    pipelineInfo.subpass = subpassIndex;

    pipelineInfo.flags = pipelineCreateFlags | ((basePipeline != VK_NULL_HANDLE) ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0) | ((canHaveDerivatives) ? VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT : 0);

    pipelineInfo.basePipelineHandle = basePipeline;
    pipelineInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(device->getInternalLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    std::vector<VkDescriptorPoolSize> poolSizes{};

    for(std::pair<VkDescriptorType, uint32_t> data : poolData) {
        VkDescriptorPoolSize size{};
        size.type = data.first;
        size.descriptorCount = data.second;
        poolSizes.push_back(size);
    }

    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapchain->getSwapchainImageCount());
    poolInfo.flags = descriptorPoolFlags;

    if (vkCreateDescriptorPool(device->getInternalLogicalDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    std::vector<VkDescriptorSetLayout> layouts(swapchain->getSwapchainImageCount(), descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchain->getSwapchainImageCount());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(swapchain->getSwapchainImageCount());
    VkResult result = vkAllocateDescriptorSets(device->getInternalLogicalDevice(), &allocInfo, descriptorSets.data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error(std::string("failed to allocate descriptor sets!\nerror: ") + std::to_string(result));
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
        throw std::runtime_error("failed to create shader");
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

VkDescriptorSetLayoutBinding& VulkanGraphicsPipeline::getDescriptorSetLayoutBinding(int index) {
    return layoutBindings.at(index);
}

void VulkanGraphicsPipeline::setDescriptorPoolData(VkDescriptorType type, uint32_t size) {
    poolData.push_back(std::make_pair(type, size));
}

void VulkanGraphicsPipeline::setPushConstantDescriptor(VkPushConstantRange desc) {
    pushContsantDescriptors.push_back(desc);
}

void VulkanGraphicsPipeline::setDescriptorSetLayoutFlags(VkDescriptorSetLayoutCreateFlags flags) {
    descriptorSetLayoutFlags = flags;
}

void VulkanGraphicsPipeline::setDescriptorPoolFlags(VkDescriptorPoolCreateFlags flags) {
    descriptorPoolFlags = flags;
}

void VulkanGraphicsPipeline::setPipelineCreateInfoFlags(VkPipelineCreateFlags flags) {
    pipelineCreateFlags = flags;
}

void VulkanGraphicsPipeline::setPipelineBase(VkPipeline base) {
    basePipeline = base;
}

void VulkanGraphicsPipeline::setCanHaveDerivatives(bool canBeParent) {
    canHaveDerivatives = canBeParent;
}

void VulkanGraphicsPipeline::setPolygonType(VkPolygonMode mode) {
    polygonType = mode;
}

void VulkanGraphicsPipeline::setLineWidth(float width) {
    lineWidth = width;
}

void VulkanGraphicsPipeline::setPrimitiveTopology(VkPrimitiveTopology top) {
    topology = top;
}

void VulkanGraphicsPipeline::setSubpassIndex(int index) {
    subpassIndex = index;
}

void VulkanGraphicsPipeline::setColorBlendAttachment(VkPipelineColorBlendAttachmentState attachment, int index) {
    if(colorBlendAttachments.size() <= index) {
        colorBlendAttachments.resize(index + 1);
    }

    colorBlendAttachments.at(index) = attachment;
}

void VulkanGraphicsPipeline::setCullMode(VkCullModeFlags mode) {
    cullMode = mode;
}

void VulkanGraphicsPipeline::setDepthTestAndWrite(bool test, bool write) {
    depthTesting = test;
    depthWriting = write;
}