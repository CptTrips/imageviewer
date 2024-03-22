#pragma once

#include <vulkan/vulkan.h>

class CommandBuffer
{

    VkDevice device;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    CommandBuffer() = default;

public:


    CommandBuffer(VkDevice device, VkCommandPool commandPool);

    CommandBuffer(const CommandBuffer&) = delete;

    CommandBuffer(CommandBuffer&&) noexcept;

    CommandBuffer& operator=(CommandBuffer) noexcept;

    ~CommandBuffer();

    VkCommandBuffer& vk();
};

