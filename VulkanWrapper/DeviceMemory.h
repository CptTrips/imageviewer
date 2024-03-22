#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

class DeviceMemory
{

    VkDevice device;
    VkDeviceMemory memory;

public:

    friend void swap(DeviceMemory& a, DeviceMemory& b);

    DeviceMemory(VkDevice device);

    DeviceMemory(Device& device, VkMemoryRequirements memRequirements);

    DeviceMemory(const DeviceMemory&) = delete;

    DeviceMemory& operator=(const DeviceMemory&) = delete;

    DeviceMemory& operator=(DeviceMemory&& other) noexcept;

    ~DeviceMemory();

    VkDeviceMemory vk() const;
};
