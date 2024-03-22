#include "Image.h"

#include <stdexcept>

void Image::createImageView(VkFormat format)
{

	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
        throw std::runtime_error("failed to create texture image view!");
}

Image::Image(
	uint32_t width,
	uint32_t height,
	uint32_t mipLevels,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
    Device& device
)
	: device(device.vk())
	, image()
	, imageView()
	, wasCreated(true)
	, memory(VK_NULL_HANDLE)
{

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(width);
	imageInfo.extent.height = static_cast<uint32_t>(height);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0; // Optional
	if (vkCreateImage(device.vk(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	memory = DeviceMemory(device, getMemoryRequirements());

	bindMemory(memory);

	createImageView(format);
}

Image::Image(Device& device, VkImage image, VkFormat format)
	: device(device.vk())
    , image(image)
	, imageView()
	, wasCreated(false)
	, memory(device.vk())
{

	createImageView(format);
}

Image::Image(Image&& other) noexcept
	: device()
	, image()
	, imageView()
	, wasCreated(false)
	, memory(other.device)
{

	swap(*this, other);
}

Image& Image::operator=(Image other) noexcept
{

	swap(*this, other);

	return *this;
}

VkImage Image::vk() const
{
    return image;
}


VkImageView Image::getImageView() const
{
	
	return imageView;
}

VkMemoryRequirements Image::getMemoryRequirements() const
{

	VkMemoryRequirements memoryRequirements;

	vkGetImageMemoryRequirements(device, image, &memoryRequirements);

	return memoryRequirements;
}

void Image::bindMemory(DeviceMemory& memory)
{

	vkBindImageMemory(device, image, memory.vk(), 0);
}

Image::~Image()
{

    vkDestroyImageView(device, imageView, nullptr);

	if (wasCreated)
		vkDestroyImage(device, image, nullptr);
}

void swap(Image& a, Image& b)
{

	std::swap(a.device, b.device);
	std::swap(a.image, b.image);
	std::swap(a.imageView, b.imageView);
	swap(a.memory, b.memory);
	std::swap(a.wasCreated, b.wasCreated);
}
