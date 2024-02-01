#pragma once

#include <string>

namespace Figment
{
    class Image
    {
    public:
        Image() = default;
        ~Image();
        uint32_t GetWidth() const
        { return m_Width; }
        uint32_t GetHeight() const
        { return m_Height; }
        void *GetData() const
        { return m_Data; }
        uint32_t GetDataSize() const
        { return m_Width * m_Height * 4; }
        static Image Load(const std::string &path);
    private:
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_Channels = 0;
        unsigned char *m_Data = nullptr;
    };
}
