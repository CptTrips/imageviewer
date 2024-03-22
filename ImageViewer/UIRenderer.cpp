#include "UIRenderer.h"

#include "imgui_stdlib.h"

#include "Device.h"
#include "GLFWWindow.h"
#include "VulkanInstance.h"
#include "SwapChain.h"


UIRenderer::UIRenderer(const UIRendererOptions& options)
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    VkPipelineRenderingCreateInfo pipelineRenderingInfo{};

    VkFormat colorAttachmentFormat = options.swapChain.getFormat();

    pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipelineRenderingInfo.colorAttachmentCount = 1;
    pipelineRenderingInfo.pColorAttachmentFormats = &colorAttachmentFormat;

	ImGui::StyleColorsDark();
    ImGui_ImplVulkan_InitInfo init_info {};
    init_info.Instance = options.instance.vk();
    init_info.PhysicalDevice = options.device.getPhysicalDevice();
    init_info.Device = options.device.vk();
    init_info.QueueFamily = options.device.findQueueFamilies().graphicsFamily.value();
    init_info.Queue = options.device.getGraphicsQueue();
    init_info.DescriptorPool = options.device.getDescriptorPool();
    init_info.UseDynamicRendering = true;
    init_info.MinImageCount = options.device.querySwapChainSupport().capabilities.minImageCount;
    init_info.ImageCount = static_cast<uint32_t>(options.swapChain.getImageCount());
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.PipelineRenderingCreateInfo = pipelineRenderingInfo;
    //init_info.CheckVkResultFn = check_vk_result;

    ImGui_ImplVulkan_Init(&init_info);
	ImGui_ImplGlfw_InitForVulkan(options.window.window, true);
}

void UIRenderer::render(CommandBuffer& commandBuffer, UI& ui)
{

    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Load Image");

    //ImGui::InputText("", onScreenImagePath.data(), );
    ImGui::InputText("Image Path", &ui.onScreenImagePath);

    ui.buttonPressed = ImGui::Button("Load");

    ImGui::End();

    ImGui::Render();

    ImDrawData* drawData = ImGui::GetDrawData();

    ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer.vk());
}

UIRenderer::~UIRenderer()
{

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
