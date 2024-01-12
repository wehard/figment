#include "Image.h"
#include "Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Figment
{
    Image::~Image()
    {
        stbi_image_free(m_Data);
    }

    Image Image::Load(const std::string &path)
    {
        Image image = {};
        int width, height, channels;
        image.m_Data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!image.m_Data)
        {
            FIG_LOG_ERROR("Failed to load image: %s", path.c_str());
        }
        image.m_Width = width;
        image.m_Height = height;
        image.m_Channels = channels;
        return image;
    }
}
