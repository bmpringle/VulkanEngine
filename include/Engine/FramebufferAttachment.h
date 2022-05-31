#ifndef FRAMEBUFFER_ATTACHMENT
#define FRAMEBUFFER_ATTACHMENT

#include "VulkanInclude.h"

struct FramebufferAttachment {
    VkFormat format;
    VkDeviceMemory memory;
    VkImage image;
    VkImageView imageView;
};

#endif