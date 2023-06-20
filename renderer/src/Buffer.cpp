#include "Buffer.h"
#include "OpenGLBuffer.h"
#include <vector>

std::shared_ptr<VertexBuffer> VertexBuffer::Create(std::vector<float> vertices)
{
    return std::make_shared<OpenGLVertexBuffer>(vertices);
}

std::shared_ptr<IndexBuffer> IndexBuffer::Create(std::vector<uint32_t> indices)
{
    return std::make_shared<OpenGLIndexBuffer>(indices);
}
