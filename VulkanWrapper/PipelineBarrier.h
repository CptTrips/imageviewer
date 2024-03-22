#pragma once

#include <vulkan/vulkan.h>

#include "Image.h"

#include "CommandBuffer.h"

struct PipelineStage
{

    VkImageLayout layout;
    VkAccessFlags accessFlags;
    VkPipelineStageFlags stageFlags;
};

class PipelineBarrier
{

    PipelineStage before, after;

public:
    PipelineBarrier(PipelineStage before, PipelineStage after);

    void layoutTransition(CommandBuffer& commandBuffer, Image& image) const;
};

