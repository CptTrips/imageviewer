#pragma once

#include <vulkan\vulkan.h>

#include <vector>

#include "VulkanDebugMessenger.h"

class VulkanInstance
{

    VkInstance instance;

    VulkanDebugMessenger db_messenger;

    bool enableValidationLayers;

	bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

	bool checkInstanceExtensionSupport(const std::vector<const char*>& extensions);

public:

    VulkanInstance(
        std::vector<const char*> instanceExtensions
        , std::vector<const char*> validationLayers = {}
    );

    VulkanInstance(const VulkanInstance&) = delete;

    VulkanInstance& operator=(VulkanInstance) = delete;

    VulkanInstance(VulkanInstance&&) /*noexcept*/ = delete;

    ~VulkanInstance();

    VkInstance vk() const;
};

