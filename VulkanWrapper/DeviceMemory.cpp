#include "DeviceMemory.h"

#include <stdexcept>
#include <utility>

DeviceMemory::DeviceMemory(VkDevice device)
	: device(device)
	, memory(VK_NULL_HANDLE)
{
}

DeviceMemory::DeviceMemory(Device& device, VkMemoryRequirements memRequirements)
	: device(device.vk())
	, memory(VK_NULL_HANDLE)
{

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(device.vk(), &allocInfo, nullptr, &memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}
}

DeviceMemory& DeviceMemory::operator=(DeviceMemory&& other) noexcept
{

	swap(*this, other);

	return *this;
}

DeviceMemory::~DeviceMemory()
{

	if (device)
        vkFreeMemory(device, memory, nullptr);

	if (!device && memory)
		std::runtime_error("DeviceMemory has memory but no device");
}

VkDeviceMemory DeviceMemory::vk() const
{

	return memory;
}

void swap(DeviceMemory& a, DeviceMemory& b)
{

	std::swap(a.memory, b.memory);

	std::swap(a.device, b.device);
}
