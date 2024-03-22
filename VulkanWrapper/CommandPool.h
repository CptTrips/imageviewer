#pragma once

#include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"

class CommandPool
{

    VkDevice device;
    VkCommandPool commandPool;

public:

    CommandPool(VkDevice device, const QueueFamilyIndices& queueFamilyIndices);

    CommandPool(const CommandPool&) = delete;

    CommandPool(CommandPool&&) = delete;

    CommandPool& operator=(const CommandPool&) = delete;

    CommandPool& operator=(CommandPool&&) = delete;//noexcept;

    ~CommandPool();

    VkCommandPool vk() const;
};

