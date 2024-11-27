#include <spdlog/spdlog.h>
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace figment
{
    Image::~Image()
    {
        stbi_image_free(m_Data);
    }

    Image Image::Load(const std::string &path)
    {
        Image image = {};
        int width, height, channels;
        image.m_Data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!image.m_Data)
        {
            spdlog::error("Failed to load image: {}", path);
        }
        image.m_Width = width;
        image.m_Height = height;
        image.m_Channels = channels;
        return image;
    }
}
