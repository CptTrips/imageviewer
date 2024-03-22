#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"
#include "DeviceMemory.h"

class Image
{

    VkDevice device;

    VkImage image;
    VkImageView imageView;

    DeviceMemory memory;

    bool wasCreated;

    void createImageView(VkFormat format);

    void bindMemory(DeviceMemory& memory);

public:

    friend void swap(Image& a, Image& b);

    Image(
        uint32_t width,
        uint32_t height,
        uint32_t mipLevels,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        Device& device
    );

    Image(Device& device, VkImage image, VkFormat format);

    Image(const Image&) = delete;

    Image(Image&& other) noexcept;

    Image& operator=(Image other) noexcept;

    VkImage vk() const;

    VkImageView getImageView() const;

    VkMemoryRequirements getMemoryRequirements() const;

    ~Image();
};

