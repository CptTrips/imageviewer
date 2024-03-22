#include "Fence.h"

#include <stdexcept>

Fence::Fence(VkDevice device)
    : device(device)
{

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (vkCreateFence(this->device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) 
        throw std::runtime_error("failed to create synchronization objects for a frame!");
}

Fence::~Fence()
{

    vkDestroyFence(device, fence, nullptr);
}

void Fence::wait() const
{

	vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
}

VkFence Fence::vk() const
{

    return fence;
}
