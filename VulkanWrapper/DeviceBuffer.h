#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

class DeviceBuffer
{
    
	VkDevice device;
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize size;

public:

	DeviceBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const Device& device);

	DeviceBuffer(const DeviceBuffer&) = delete;

	DeviceBuffer& operator=(const DeviceBuffer&) = delete;

	DeviceBuffer& operator=(DeviceBuffer&&) noexcept;

	~DeviceBuffer();

	void fill(void* data);

	VkBuffer vk() const;
};

