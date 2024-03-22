#pragma once

#include <vector>
#include <memory>
#include <optional>

#include "DeviceBuffer.h"
#include "Buffer.h"
#include "VulkanInstance.h"
#include "GLFWWindow.h"
#include "Surface.h"
#include "Device.h"
#include "SwapChain.h"
#include "UI.h"
#include "UIRenderer.h"
#include "PipelineBarrier.h"
#include "Image.h"

struct BufferRendererOptions
{

    GLFWWindow& window;

    BufferRendererOptions(GLFWWindow& window)
        : window(window)
    {}
};

class BufferRenderer
{

    static const std::vector<const char*> validationLayers;
    static const std::vector<const char*> deviceExtensions;

    static const std::vector<PipelineStage> pipelineStages;

    GLFWWindow& window;

    VulkanInstance instance;

    Surface surface;

    Device device;

    SwapChain swapChain;

    std::unique_ptr<DeviceBuffer> p_stagingBuffer;

    const std::vector<PipelineBarrier> pipelineBarriers;

    UIRenderer uiRenderer;

    Image bufferImage;

    std::vector<PipelineBarrier> createPipelineBarriers() const;

    std::vector<const char*> getRequiredInstanceExtensions();

	void copyDeviceBufferToImage(CommandBuffer& commandBuffer, const DeviceBuffer& buffer, const Image& image, uint32_t width, uint32_t height);

    void recordRenderCommands(CommandBuffer& commandBuffer, Image & image, UI& ui);

    void beginRendering(CommandBuffer& commandBuffer, const Image& image) const;

    void blitBufferImageToSwapChainImage(CommandBuffer& commandBuffer, const Image& swapChainImage) const;

    Image createBufferImage(uint32_t width, uint32_t height);

public:
    BufferRenderer(BufferRendererOptions options);

    void submitBuffer(Buffer& buffer);

    void render(UI& ui);
};

