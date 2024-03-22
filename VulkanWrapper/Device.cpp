#include "Device.h"

#include <stdexcept>
#include <iostream>
#include <unordered_set>
#include <array>

Device::~Device()
{

	destroyDescriptorPool();

	destroyCommandPool();

	vkDestroyDevice(device, nullptr);
}

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{

	QueueFamilyIndices indices{ 0, 0 };

	uint32_t queueFamilyCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{

		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

		if (presentSupport)
			indices.presentFamily = i;

		if (indices.isComplete())
			break;

		i++;
	}

	return indices;
}

void Device::createDescriptorPool(int maxFramesInFlight)
{

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(maxFramesInFlight);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void Device::destroyDescriptorPool()
{

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

QueueFamilyIndices Device::findQueueFamilies() const
{

	return findQueueFamilies(physicalDevice, surface);
}

VkPhysicalDeviceProperties Device::getPhysicalDeviceProperties() const
{

	VkPhysicalDeviceProperties physicalDeviceProperties {};

	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
	return physicalDeviceProperties;
}

VkFormat Device::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{

	for (VkFormat format : candidates) {

		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

bool Device::checkLinearBlittSupport(VkFormat imageFormat) const
{

    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

	return (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{

		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type");
}

VkDevice Device::vk() const
{
	return device;
}

SwapChainSupportDetails Device::querySwapChainSupport() const
{

	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

	if (formatCount)
	{
		details.formats.resize(formatCount);

		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
	}

	return details;
}

VkQueue Device::getPresentQueue() const
{

	return presentQueue;
}

VkQueue Device::getGraphicsQueue() const
{

	return graphicsQueue;
}

CommandBuffer Device::makeSingleUseCommandBuffer()
{

	return CommandBuffer(device, commandPool);
}

void Device::submitCommandBuffer(CommandBuffer& commandBuffer)
{

	vkEndCommandBuffer(commandBuffer.vk());

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer.vk();

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
}

void Device::graphicsQueueWaitIdle() const
{

	vkQueueWaitIdle(graphicsQueue);
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{

		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type");
}

VkPhysicalDevice Device::getPhysicalDevice() const
{

	return physicalDevice;
}

VkDescriptorPool Device::getDescriptorPool() const
{

	return descriptorPool;
}

bool Device::checkPhysicalDevice(VkPhysicalDevice device)
{

	if (queueFamilyIndices = findQueueFamilies(device, surface); !queueFamilyIndices.isComplete())
		return false;

	return checkPhysicalDeviceFeatures(device) && checkPhysicalDeviceProperties(device);
}

bool Device::checkPhysicalDeviceFeatures(VkPhysicalDevice device) const
{

	VkPhysicalDeviceFeatures2 features2{};
	features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

	VkPhysicalDeviceVulkan13Features vulkan13Features{};
	vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

	features2.pNext = &vulkan13Features;

	vkGetPhysicalDeviceFeatures2(device, &features2);

	return
		vulkan13Features.dynamicRendering == VK_TRUE
		&& vulkan13Features.synchronization2 == VK_TRUE;

}

bool Device::checkPhysicalDeviceProperties(VkPhysicalDevice device) const
{

	VkPhysicalDeviceProperties2 properties{};
	properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

	vkGetPhysicalDeviceProperties2(device, &properties);

	return true;
}

VkPhysicalDevice Device::pickPhysicalDevice(VkInstance instance)
{

	uint32_t device_count = 0;

	vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

	if (device_count == 0)
		throw std::runtime_error("No device with Vulkan support");

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

	VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

	int i = 0;
	for (const auto& device : devices)
	{
		std::cout << std::endl << "Device " << i << std::endl;

		//if (queueFamilyIndices = findQueueFamilies(device, surface); queueFamilyIndices.isComplete())
		if (checkPhysicalDevice(device))
		{

			physicalDevice = device;

			break;
		}

		i++;
	}


	if (physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find suitable physical device");

	return physicalDevice;
}

VkDevice Device::createDevice(const std::vector<const char*>& deviceExtensions, const std::vector<const char*>& validationLayers)
{

	VkDevice device;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::unordered_set<uint32_t> uniqueQueueFamilies = { queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value() };

	float queuePriority = 1.f;

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;

		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkPhysicalDeviceVulkan13Features vk13Features{};
	vk13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	vk13Features.dynamicRendering = VK_TRUE;
	vk13Features.synchronization2 = VK_TRUE;

	VkPhysicalDeviceFeatures2 deviceFeatures2{};
	deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures2.features = deviceFeatures;
	deviceFeatures2.pNext = &vk13Features;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = &deviceFeatures2;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = VK_NULL_HANDLE;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (validationLayers.size() > 0) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	return device;
}

void Device::getQueues()
{

	vkGetDeviceQueue(device, queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);

	vkGetDeviceQueue(device, queueFamilyIndices.presentFamily.value(), 0, &presentQueue);
}

void Device::createCommandPool()
{

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(this->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void Device::destroyCommandPool()
{

	vkDestroyCommandPool(device, commandPool, nullptr);
}

Device::Device(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions, const std::vector<const char*>& validationLayers)
	: queueFamilyIndices()
	, surface(surface)
	, physicalDevice(pickPhysicalDevice(instance))
	, device(createDevice(deviceExtensions, validationLayers))
	, graphicsQueue()
	, presentQueue()
	, commandPool()
	, descriptorPool()
{

	getQueues();

	createCommandPool();

	createDescriptorPool(1);
}
