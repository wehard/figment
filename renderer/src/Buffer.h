#pragma once

#include <memory>
#include <stdint.h>

class VertexBuffer
{
public:
    virtual ~VertexBuffer() = default;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    static std::shared_ptr<VertexBuffer> Create(std::vector<float> vertices);

private:
    uint32_t m_ID;
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
