#include "CommandBuffer.h"

#include <utility>
#include <stdexcept>

CommandBuffer::CommandBuffer(VkDevice device, VkCommandPool commandPool)
    : device(device)
    , commandPool(commandPool)
    , commandBuffer()
{

	VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer!");
	
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : device(other.device)
    , commandPool(other.commandPool)
{

    std::swap(commandBuffer, other.commandBuffer);
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer other) noexcept
{

    std::swap(device, other.device);
    std::swap(commandPool, other.commandPool);
    std::swap(commandBuffer, other.commandBuffer);

    return *this;
}

CommandBuffer::~CommandBuffer()
{

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

VkCommandBuffer& CommandBuffer::vk()
{

    return commandBuffer;
}

