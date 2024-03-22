#pragma once

#include <vulkan\vulkan.h>

class FrameBuffer
{

    const VkDevice device;
    VkFramebuffer frameBuffer;
    VkImageView imageView;

    void createImageView(VkImage image, VkFormat format);

    void createFrameBuffer();

public:

    FrameBuffer();

    FrameBuffer(const VkDevice device, VkImage image, VkFormat format);

    FrameBuffer(const FrameBuffer&) = delete;

    FrameBuffer& operator=(const FrameBuffer&) = delete;

    FrameBuffer(FrameBuffer&& other) noexcept = delete;

    FrameBuffer& operator=(FrameBuffer && other) = delete;

    ~FrameBuffer();
};

