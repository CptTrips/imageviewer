#pragma once

#include <vulkan\vulkan.h>

class Fence
{

    VkFence fence;

    VkDevice device;

public:

    Fence(VkDevice device);

    Fence(const Fence&) = delete;

    Fence(Fence&&) noexcept = delete; // Implement this

    Fence& operator=(const Fence&) = delete;

    Fence& operator=(Fence&&) noexcept = delete; // Implement this

    ~Fence();

    void wait() const;

    VkFence vk() const;
};

