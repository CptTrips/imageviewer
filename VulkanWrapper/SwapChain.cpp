#include "SwapChain.h"

#include <stdexcept>

#include "Fence.h"

void SwapChain::acquireSwapChainImages()
{

	uint32_t imageCount;

	vkGetSwapchainImagesKHR(device->vk(), swapChain, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(device->vk(), swapChain, &imageCount, images.data());

}


SwapChain::SwapChain(Device& device, const Surface& surface, const GLFWWindow& window)
	: SwapChain(device, makeSwapChainOptions(device, surface.getVkSurface(), window))
{
}

SwapChain::SwapChain(Device& device, SwapChainOptions options)
	: SwapChain(device, options.surface, options.surfaceFormat, options.extent, options.presentMode, options.imageCount, options.preTransform, options.indices)
{
}

SwapChain::SwapChain(Device& inDevice, VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkExtent2D extent, VkPresentModeKHR presentMode, uint32_t imageCount, VkSurfaceTransformFlagBitsKHR preTransform, QueueFamilyIndices indices)
	: device(&inDevice)
	, presentQueue(inDevice.getPresentQueue())
	, format(surfaceFormat.format)
	, extent(extent)
{

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = preTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device->vk(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain");
	}

	acquireSwapChainImages();
}

SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
{

	std::swap(device, other.device);

	std::swap(presentQueue, other.presentQueue);

	std::swap(swapChain, other.swapChain);

	std::swap(images, other.images);

	std::swap(extent, other.extent);

	std::swap(format, other.format);

	return *this;
}

SwapChain::~SwapChain()
{

    vkDestroySwapchainKHR(device->vk(), swapChain, nullptr);
}

VkFormat SwapChain::getFormat() const
{
	return format;
}

VkExtent2D SwapChain::getExtent() const
{
	return extent;
}

uint32_t SwapChain::getFreeImageIndex() const
{

	uint32_t imageIndex;

	Fence imageAvailableFence(device->vk());

	VkResult result = vkAcquireNextImageKHR(
		device->vk(),
		swapChain,
		UINT64_MAX,
		VK_NULL_HANDLE,//imageAvailableSemaphores[currentFrame],
		imageAvailableFence.vk(),
		&imageIndex
	);

	imageAvailableFence.wait();

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	return imageIndex;
}

Image SwapChain::getImage(uint32_t imageIndex) const
{

	return Image(*device, images[imageIndex], format);
}

void SwapChain::queueImage(uint32_t imageIndex)
{

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 0;
	//presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];

	VkSwapchainKHR swapChains[] = {swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result != VK_SUCCESS)
	{

		throw std::runtime_error("Failed to present swap chain image!");
	}

}

size_t SwapChain::getImageCount() const
{

	return images.size();
}

SwapChainOptions SwapChain::makeSwapChainOptions(Device& device, VkSurfaceKHR surface, const GLFWWindow& window) const
{

	SwapChainOptions options;

	SwapChainSupportDetails swapChainSupport = device.querySwapChainSupport();

	options.surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

	options.extent = chooseSwapExtent(swapChainSupport.capabilities, window);

	options.presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

	options.imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && options.imageCount > swapChainSupport.capabilities.maxImageCount)
		options.imageCount = swapChainSupport.capabilities.maxImageCount;

	options.indices = device.findQueueFamilies();

	options.preTransform = swapChainSupport.capabilities.currentTransform;

	options.surface = surface;

	return options;
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
{

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}

	throw std::runtime_error("No appropriate surface format support");
	
	return VkSurfaceFormatKHR();
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
{

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const GLFWWindow& window) const
{

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window.window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}
