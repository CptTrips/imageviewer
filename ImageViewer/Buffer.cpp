#define STB_IMAGE_IMPLEMENTATION
#include "Buffer.h"

#include <utility>

int Buffer::getWidth() const
{

    return width;
}

int Buffer::getHeight() const
{

    return height;
}

stbi_uc* Buffer::getData() const
{

    return data;
}

Buffer::Buffer()
    : width(0)
    , height(0)
    , channels(0)
    , data(nullptr)
{
}

Buffer::Buffer(std::string path)
{

    data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
}

Buffer::Buffer(Buffer&& other) noexcept
    : width(other.width)
    , height(other.height)
    , channels(other.channels)
    , data(other.data)
{

    other.data = nullptr;
}

Buffer& Buffer::operator=(Buffer other) noexcept
{

    std::swap(width, other.width);
    std::swap(height, other.height);
    std::swap(channels, other.channels);
    std::swap(data, other.data);

    return *this;
}

Buffer::~Buffer()
{

    stbi_image_free(data);
}
