#include "PipelineBarrier.h"

PipelineBarrier::PipelineBarrier(PipelineStage before, PipelineStage after)
    : before(before)
    , after(after)
{
}

void PipelineBarrier::layoutTransition(CommandBuffer& commandBuffer, Image& image) const
{

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = before.layout;
	barrier.newLayout = after.layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.srcAccessMask = before.accessFlags;
	barrier.dstAccessMask = after.accessFlags;
	barrier.image = image.vk();
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;


	vkCmdPipelineBarrier(
		commandBuffer.vk(),
		before.stageFlags, after.stageFlags,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
}
