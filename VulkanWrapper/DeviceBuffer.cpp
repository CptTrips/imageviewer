#include "DeviceBuffer.h"

#include <stdexcept>

DeviceBuffer::DeviceBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const Device& device)
    : device(device.vk())
    , buffer()
    , memory()
    , size(size)
{

	VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.vk(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.vk(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device.vk(), &allocInfo, nullptr, &memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device.vk(), buffer, memory, 0);
}

DeviceBuffer& DeviceBuffer::operator=(DeviceBuffer&& other) noexcept
{

    using std::swap;

    swap(device, other.device);
    swap(buffer, other.buffer);
    swap(memory, other.memory);
    swap(size, other.size);

    return *this;
}

DeviceBuffer::~DeviceBuffer()
{

	vkDestroyBuffer(device, buffer, nullptr);
    vkFreeMemory(device, memory, nullptr);
}

void DeviceBuffer::fill(void* data)
{

	void* deviceData;

	vkMapMemory(device,  memory, 0, size, 0, &deviceData);
	memcpy(deviceData, data, static_cast<size_t>(size));
	vkUnmapMemory(device, memory);
}

VkBuffer DeviceBuffer::vk() const
{

    return buffer;
}
