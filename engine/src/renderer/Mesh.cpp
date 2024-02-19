#include "Mesh.h"

namespace Figment
{
    Mesh::Mesh(WGPUDevice device, const std::vector<Vertex> &vertices, std::vector<uint32_t> indices) : m_Vertices(
            vertices),
            m_Indices(indices)
    {
        m_VertexBuffer = new WebGPUVertexBuffer<Vertex>(device, "MeshVertexBuffer",
                vertices.size() * sizeof(Vertex));
        auto vertexAttributes = std::vector<WGPUVertexAttribute>({
                {
                        .format = WGPUVertexFormat_Float32x3,
                        .offset = 0,
                        .shaderLocation = 0,
                }
        });
        m_VertexBuffer->SetVertexLayout(vertexAttributes, sizeof(Vertex), WGPUVertexStepMode_Vertex);
        m_VertexBuffer->SetData(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
        m_IndexBuffer = new WebGPUIndexBuffer<uint32_t>(device, "MeshIndexBuffer",
                indices.size() * sizeof(uint32_t));
        m_IndexBuffer->SetData(indices.data(), indices.size() * sizeof(uint32_t));

        m_UniformBuffer = new WebGPUUniformBuffer<glm::mat4>(device, "MeshUniformBuffer", sizeof(glm::mat4));
    }
}
