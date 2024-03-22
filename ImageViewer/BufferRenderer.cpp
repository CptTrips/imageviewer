#include "BufferRenderer.h"

#include <vulkan/vulkan.h>

#include "CommandBuffer.h"

const std::vector<const char*> BufferRenderer::validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> BufferRenderer::deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<PipelineStage> BufferRenderer::pipelineStages
{

	{VK_IMAGE_LAYOUT_UNDEFINED, 0, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT},
	{VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT},
	{VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_NONE},
	{VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT}
};

BufferRenderer::BufferRenderer(BufferRendererOptions options)
	: window(options.window)
#ifdef NDEBUG
	, instance(getRequiredInstanceExtensions())
#else
	, instance(getRequiredInstanceExtensions(), validationLayers)
#endif
	, surface(instance.vk(), window.window)
	, device(instance.vk(), surface.getVkSurface(), deviceExtensions, validationLayers)
	, swapChain(device, surface, window)
	, p_stagingBuffer()
	, pipelineBarriers(createPipelineBarriers())
	, uiRenderer({ window, device, instance, swapChain })
	, bufferImage(createBufferImage(1,1))
{

	device.getPhysicalDeviceProperties();
}

void BufferRenderer::submitBuffer(Buffer& buffer)
{

	window.resize(buffer.getWidth(), buffer.getHeight());

	swapChain = SwapChain(device, surface, window);

	VkDeviceSize imageSize = buffer.getWidth() * buffer.getHeight() * 4; // 4 for RGBA

	p_stagingBuffer = std::make_unique<DeviceBuffer>(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, device);

	p_stagingBuffer->fill(buffer.getData());

	bufferImage = createBufferImage(buffer.getWidth(), buffer.getHeight());

	std::vector<PipelineStage> stages{
		{ VK_IMAGE_LAYOUT_UNDEFINED, 0, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT},
		{ VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT },
		{ VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT }
	};

	PipelineBarrier prepareAsDst(stages[0], stages[1]);

	PipelineBarrier prepareAsSrc(stages[1], stages[2]);

	CommandBuffer commandBuffer{ device.makeSingleUseCommandBuffer() };

	prepareAsDst.layoutTransition(commandBuffer, bufferImage);

	copyDeviceBufferToImage(commandBuffer, *p_stagingBuffer, bufferImage, swapChain.getExtent().width, swapChain.getExtent().height);

	prepareAsSrc.layoutTransition(commandBuffer, bufferImage);

	device.submitCommandBuffer(commandBuffer);

	device.graphicsQueueWaitIdle();
}


std::vector<PipelineBarrier> BufferRenderer::createPipelineBarriers() const
{

	std::vector<PipelineBarrier> pipelineBarriers;

	PipelineStage prevStage{ pipelineStages[0] };

	for (auto it = pipelineStages.begin() + 1; it != pipelineStages.end(); it++)
	{

		pipelineBarriers.emplace_back(prevStage, *it);

		prevStage = *it;
	}

	return pipelineBarriers;
}

std::vector<const char*> BufferRenderer::getRequiredInstanceExtensions()
{

	std::vector<const char*> extensions = window.extensions();

	return extensions;
}


void BufferRenderer::copyDeviceBufferToImage(CommandBuffer& commandBuffer, const DeviceBuffer& buffer, const Image& image, uint32_t width, uint32_t height)
{

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = {0, 0, 0};
	VkExtent2D swapChainExtent{ swapChain.getExtent() };
	region.imageExtent = {
		swapChainExtent.width,
		swapChainExtent.height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer.vk(),
		buffer.vk(),
		image.vk(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

}

void BufferRenderer::render(UI& ui)
{

	// Acquire next available rendering resource pack (image, command buffer...)
	uint32_t imageIndex{ swapChain.getFreeImageIndex() };
	Image image(swapChain.getImage(imageIndex));

	// Record rendering instructions in command buffer
	CommandBuffer commandBuffer(device.makeSingleUseCommandBuffer());

	recordRenderCommands(commandBuffer, image, ui);

	// Submit command buffer
	device.submitCommandBuffer(commandBuffer);

	// Queue rendered frame
	swapChain.queueImage(imageIndex);

	device.graphicsQueueWaitIdle();
}

void BufferRenderer::recordRenderCommands(CommandBuffer& commandBuffer, Image & image, UI& ui)
{

	pipelineBarriers[0].layoutTransition(commandBuffer, image);

	blitBufferImageToSwapChainImage(commandBuffer, image);

	pipelineBarriers[1].layoutTransition(commandBuffer, image);

	beginRendering(commandBuffer, image);

	uiRenderer.render(commandBuffer, ui);

	vkCmdEndRendering(commandBuffer.vk());

	pipelineBarriers[2].layoutTransition(commandBuffer, image);
}

void BufferRenderer::beginRendering(CommandBuffer & commandBuffer, const Image & image) const
{

	VkRenderingAttachmentInfo colourAttachmentInfo{};
	
	colourAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colourAttachmentInfo.imageView = image.getImageView();
	colourAttachmentInfo.imageLayout = pipelineStages[2].layout;
	colourAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
	colourAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	colourAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//colourAttachmentInfo.clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };

	VkRenderingInfo renderInfo{};

	renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderInfo.renderArea.offset = { 0, 0 };
	renderInfo.renderArea.extent = swapChain.getExtent();
	renderInfo.layerCount = 1;
	renderInfo.viewMask = 0;
	renderInfo.colorAttachmentCount = 1;
	renderInfo.pColorAttachments = &colourAttachmentInfo;
	renderInfo.pDepthAttachment = VK_NULL_HANDLE;
	renderInfo.pStencilAttachment = VK_NULL_HANDLE;
	
	vkCmdBeginRendering(commandBuffer.vk(), &renderInfo);

}

void BufferRenderer::blitBufferImageToSwapChainImage(CommandBuffer& commandBuffer, const Image& swapChainImage) const
{

	VkBlitImageInfo2 blitInfo{};

	blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;

	blitInfo.srcImage = bufferImage.vk();
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.dstImage = swapChainImage.vk();
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	blitInfo.regionCount = 1;

	VkImageBlit2 regionInfo{};

	regionInfo.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

	VkImageSubresourceLayers srcSubLayer{}, dstSubLayer{};

	srcSubLayer.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	srcSubLayer.mipLevel = 0;
	srcSubLayer.baseArrayLayer = 0;
	srcSubLayer.layerCount = 1;

	dstSubLayer.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	dstSubLayer.mipLevel = 0;
	dstSubLayer.baseArrayLayer = 0;
	dstSubLayer.layerCount = 1;

	regionInfo.srcSubresource = srcSubLayer;
	regionInfo.dstSubresource = dstSubLayer;

	VkExtent2D extent{ swapChain.getExtent() };

	VkOffset3D start{ 0, 0, 0 }, end{ static_cast<int32_t>(extent.width), static_cast<int32_t>(extent.height), 1 };

	regionInfo.srcOffsets[0] = start;
    regionInfo.srcOffsets[1] = end;

	regionInfo.dstOffsets[0] = start;
	regionInfo.dstOffsets[1] = end;

	blitInfo.pRegions = &regionInfo;

	blitInfo.filter = VK_FILTER_NEAREST;

	vkCmdBlitImage2(commandBuffer.vk(), &blitInfo);
}

Image BufferRenderer::createBufferImage(uint32_t width, uint32_t height)
{

	return Image(
		width,
		height,
		1,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		device
	);
}
