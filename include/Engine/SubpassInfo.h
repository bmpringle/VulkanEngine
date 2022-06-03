#ifndef SUBPASSINFO_H
#define SUBPASSINFO_H

#include "VulkanInclude.h"

struct SubpassInfo {
    std::vector<VkAttachmentReference> colorAttachments;
    std::vector<VkAttachmentReference> inputAttachments;
    VkAttachmentReference depthAttachment;

    void populateDescription() {
        desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        desc.colorAttachmentCount = colorAttachments.size();
        desc.pColorAttachments = colorAttachments.data();
        desc.pDepthStencilAttachment = &depthAttachment;
        desc.inputAttachmentCount = inputAttachments.size();
        desc.pInputAttachments = inputAttachments.data();
    }

    VkSubpassDescription desc;
};

#endif