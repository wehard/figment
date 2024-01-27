#pragma once

#include <vector>
#include "WebGPUBuffer.h"
#include "WebGPUContext.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

namespace Figment
{
    struct Vertex
    {
        glm::vec3 Position;
    };

    class Mesh
    {
    public:
        Mesh(WGPUDevice device, std::vector<Vertex> vertices, std::vector<uint32_t> indices);
        WebGPUVertexBuffer<Vertex> *VertexBuffer()
        { return m_VertexBuffer; }
        WebGPUIndexBuffer<uint32_t> *IndexBuffer()
        { return m_IndexBuffer; }
        WebGPUUniformBuffer<glm::mat4> *UniformBuffer()
        { return m_UniformBuffer; }
    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
        WebGPUVertexBuffer<Vertex> *m_VertexBuffer;
        WebGPUIndexBuffer<uint32_t> *m_IndexBuffer;
        WebGPUUniformBuffer<glm::mat4> *m_UniformBuffer;
    };
}
