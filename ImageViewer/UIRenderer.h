#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "CommandBuffer.h"
#include "GLFWWindow.h"
#include "Device.h"
#include "VulkanInstance.h"
#include "SwapChain.h"
#include "UI.h"


struct UIRendererOptions
{

    const GLFWWindow& window;
    const Device& device;
    const VulkanInstance& instance;
    const SwapChain& swapChain;

    UIRendererOptions(
        const GLFWWindow& window,
        const Device& device,
        const VulkanInstance& instance,
        const SwapChain& swapChain
    )
        : window(window)
        , device(device)
        , instance(instance)
        , swapChain(swapChain)
    {}
};


class UIRenderer
{

public:
    UIRenderer(const UIRendererOptions& options);

    void render(CommandBuffer& commandBuffer, UI& ui);

    ~UIRenderer();
};

