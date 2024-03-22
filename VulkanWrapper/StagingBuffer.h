#pragma once

#include <vulkan/vulkan.h>

class StagingBuffer
{

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

public:

	StagingBuffer();

	StagingBuffer(const StagingBuffer&) = delete;

	StagingBuffer& operator=(const StagingBuffer&) = delete;

	StagingBuffer& operator=(StagingBuffer&&) noexcept;

	~StagingBuffer();
};

