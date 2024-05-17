#pragma once

#include <cstdint>

namespace Figment
{
    enum class BufferUsage
    {
        None = 0x00000000,
        MapRead = 0x00000001,
        MapWrite = 0x00000002,
        CopySrc = 0x00000004,
        CopyDst = 0x00000008,
        Index = 0x00000010,
        Vertex = 0x00000020,
        Uniform = 0x00000040,
        Storage = 0x00000080,
    };

    struct BufferDescriptor
    {
        uint32_t ByteSize = 0;
        BufferUsage Usage = BufferUsage::None;
    };

    class Buffer
    {
    public:
        Buffer() = default;
        explicit Buffer(const BufferDescriptor &&descriptor)
        {
            m_ByteSize = descriptor.ByteSize;
        };
        ~Buffer() = default;

        [[nodiscard]] uint32_t GetByteSize() const { return m_ByteSize; }

        static Buffer Create(uint32_t sizeBytes, BufferUsage usage)
        {
            return Buffer({ .ByteSize = sizeBytes, .Usage = usage });
        }

    private:
        uint32_t m_ByteSize = 0;
    };
}
