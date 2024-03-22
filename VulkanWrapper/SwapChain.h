#pragma once

#include <vulkan\vulkan.h>

#include <vector>

#include "Device.h"
#include "FrameBuffer.h"
#include "Surface.h"
#include "GLFWWindow.h"
#include "Image.h"

struct SwapChainOptions
{

    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D extent;
    VkPresentModeKHR presentMode;
    uint32_t imageCount;
    VkSurfaceTransformFlagBitsKHR preTransform;
    QueueFamilyIndices indices;
};

// Manages an array of image data which will be drawn to by the render process and sent to the display
class SwapChain
{

    Device* device;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> images;
    VkFormat format;
    VkExtent2D extent;

    SwapChainOptions makeSwapChainOptions(Device& device, VkSurfaceKHR surface, const GLFWWindow& window) const;

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const GLFWWindow& window) const;

    void acquireSwapChainImages();

public:

    SwapChain(Device& device, const Surface& surface, const GLFWWindow& window);

    SwapChain(Device& device, SwapChainOptions options);

    SwapChain(Device& device, VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkExtent2D extent, VkPresentModeKHR presentMode, uint32_t imageCount, VkSurfaceTransformFlagBitsKHR preTransform, QueueFamilyIndices indices);

    SwapChain(const SwapChain&) = delete;

    SwapChain& operator=(const SwapChain&) = delete;

    // SwapChain(SwapChain&& other) noexcept;

    SwapChain& operator=(SwapChain&&) noexcept;

    ~SwapChain();

    VkFormat getFormat() const;

    VkExtent2D getExtent() const;

    uint32_t getFreeImageIndex() const;

    Image getImage(uint32_t imageIndex) const;

    void queueImage(uint32_t imageIndex);

    size_t getImageCount() const;
};

