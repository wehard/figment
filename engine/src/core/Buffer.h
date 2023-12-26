#pragma once

#include <memory>
#include <vector>
#include <stdint.h>

enum class BufferElementType
{
    Int,
    Float,
};

struct BufferElement
{
    BufferElementType Type;
    uint32_t Size;
    uint32_t Offset;
};

class BufferLayout
{
public:
    BufferLayout() {}
    BufferLayout(std::initializer_list<BufferElement> elements) : m_Elements(elements)
    {
        size_t offset = 0;
        for (auto e : elements)
        {
            e.Offset = offset;
            offset += e.Size;
            m_Stride += e.Size;
        }
    }

private:
    std::vector<BufferElement> m_Elements;
    uint32_t m_Stride = 0;
};

class VertexBuffer
{
public:
    virtual ~VertexBuffer() = default;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void SetLayout(BufferLayout layout) = 0;
    virtual BufferLayout GetLayout() = 0;

    static std::shared_ptr<VertexBuffer> Create(std::vector<float> vertices);

private:
    uint32_t m_ID;
    BufferLayout m_Layout;
};

class IndexBuffer
{
public:
    virtual ~IndexBuffer() = default;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    static std::shared_ptr<IndexBuffer> Create(std::vector<uint32_t> indices);

private:
    uint32_t m_ID;
};
