#ifndef ATTACHMENTDESCRIPTIONINFO_H
#define ATTACHMENTDESCRIPTIONINFO_H

#include "VulkanInclude.h"

struct AttachmentDescriptionInfo {
    //required to specify
    int attachmentIndex;
    VkImageLayout finalLayout;

    //optional to specify
    VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

};

#endif