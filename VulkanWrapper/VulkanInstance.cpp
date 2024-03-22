#include "VulkanInstance.h"

#include <stdexcept>
#include <unordered_set>
#include <string>
#include <iostream>

bool VulkanInstance::checkValidationLayerSupport(const std::vector<const char*>& requestedValidationLayers)
{

	if (requestedValidationLayers.empty())
		return true;

	std::unordered_set<std::string> requestedLayerSet(requestedValidationLayers.cbegin(), requestedValidationLayers.cend());

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const auto& layer_properties : availableLayers)
	{

		if (auto it = requestedLayerSet.find(layer_properties.layerName); it != requestedLayerSet.cend())
			requestedLayerSet.erase(it);
	}

	return requestedLayerSet.empty();
}

bool VulkanInstance::checkInstanceExtensionSupport(const std::vector<const char*>& extensions)
{

	if (extensions.empty())
		return true;

	std::unordered_set<std::string> extensionSet(extensions.cbegin(), extensions.cend());

	uint32_t supported_extension_count = 0;

	vkEnumerateInstanceExtensionProperties(nullptr, &supported_extension_count, nullptr);

	std::vector<VkExtensionProperties> supported_extensions(supported_extension_count);

	vkEnumerateInstanceExtensionProperties(nullptr, &supported_extension_count, supported_extensions.data());

	for (const auto& extension_properties : supported_extensions)
	{

		if (auto it = extensionSet.find(extension_properties.extensionName); it != extensionSet.cend())
			extensionSet.erase(it);
	}

	return extensionSet.empty();
}


VulkanInstance::VulkanInstance(
    std::vector<const char*> instanceExtensions
    , std::vector<const char*> validationLayers
)
	: enableValidationLayers(validationLayers.size() > 0)
{
	
	if (!checkValidationLayerSupport(validationLayers))
		throw std::runtime_error("Requested Validation Layers not available");

	VkApplicationInfo appInfo{};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // structs are typically part of void* linked lists so each struct must keep track of its type
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    if (enableValidationLayers)
    {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    if (!checkInstanceExtensionSupport(instanceExtensions))
        throw std::runtime_error("Some extensions not supported");

    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;


	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (result != VK_SUCCESS)
		throw std::runtime_error("VkInstance creation failed");

	db_messenger = VulkanDebugMessenger(instance);

}

VulkanInstance::~VulkanInstance()
{

	db_messenger.destroy(instance);

	vkDestroyInstance(instance, nullptr);
}

VkInstance VulkanInstance::vk() const
{

	return instance;
}
