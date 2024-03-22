#pragma once

#include <string>

#include "stb_image.h"

#include <vulkan/vulkan.h>

#include "Image.h"

class Buffer
{

    int width, height, channels;

    stbi_uc* data;


public:

    Buffer();
    Buffer(std::string path);

    int getWidth() const;

    int getHeight() const;

    stbi_uc* getData() const;

    Buffer(const Buffer&) = delete;

    Buffer(Buffer&&) noexcept;
    Buffer& operator=(Buffer) noexcept;

    ~Buffer();
};

