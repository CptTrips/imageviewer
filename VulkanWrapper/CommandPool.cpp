#include "CommandPool.h"

#include <stdexcept>
#include <utility>

CommandPool::CommandPool(VkDevice device, const QueueFamilyIndices& queueFamilyIndices)
    : device(device)
{

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(this->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

/*
CommandPool& CommandPool::operator=(CommandPool&& other) noexcept
{

	device = other.device;

	std::swap(commandPool, other.commandPool);
}*/

CommandPool::~CommandPool()
{

	vkDestroyCommandPool(device, commandPool, nullptr);
}

VkCommandPool CommandPool::vk() const
{

	return commandPool;
}
