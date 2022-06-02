#ifndef FRAMEBUFFERATTACHMENTINFO_H
#define FRAMEBUFFERATTACHMENTINFO_H

#include "VulkanInclude.h"

enum ATTACHMENT_COUNT {
    SINGLE,
    SWAPCHAIN_COUNT
};

struct FramebufferAttachmentInfo {
    //must be specified
    std::vector<VkFormat> formatCandidates;
    VkFormatFeatureFlags formatFeatures;
    int usageBits; //VkImageUsageFlagBits
    int imageAspectBits; //VkImageAspectFlagBits
    int index;

    //optional
    VkImageTiling imageTiling = VK_IMAGE_TILING_OPTIMAL;
    VkMemoryPropertyFlagBits memoryPropertyBits = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    ATTACHMENT_COUNT count = SINGLE;
};

#endif