#ifndef FRAMEBUFFERATTACHMENTINFO_H
#define FRAMEBUFFERATTACHMENTINFO_H

#include "VulkanInclude.h"

struct FramebufferAttachmentInfo {
    //must be specified
    std::vector<VkFormat> formatCandidates;
    VkFormatFeatureFlags formatFeatures;
    int usageBits; //VkImageUsageFlagBits
    int imageAspectBits; //VkImageAspectFlagBits
    std::string name;

    //optional
    VkImageTiling imageTiling = VK_IMAGE_TILING_OPTIMAL;
    VkMemoryPropertyFlagBits memoryPropertyBits = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    int count = 1; //if set to -1, the Swapchain will make as many as there are images in the swapchain
};

#endif