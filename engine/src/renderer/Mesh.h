#pragma once

#include <vector>
#include "WebGPUBuffer.h"
#include "WebGPUContext.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

namespace Figment
{

    class Mesh
    {
    public:
        struct Vertex
        {
            glm::vec3 Position;
            glm::vec2 TexCoord;

            static std::vector<WGPUVertexAttribute> GetAttributes()
            {
                return {{
                                .format = WGPUVertexFormat_Float32x3,
                                .offset = 0,
                                .shaderLocation = 0
                        },
                        {
                                .format = WGPUVertexFormat_Float32x2,
                                .offset = 12,
                                .shaderLocation = 1
                        }
                };
            }
            static WGPUVertexBufferLayout GetLayout(WGPUVertexStepMode stepMode)
            {
                auto attributes = GetAttributes();
                WGPUVertexBufferLayout layout = {};
                layout.attributeCount = attributes.size();
                layout.attributes = attributes.data();
                layout.arrayStride = sizeof(Vertex);
                layout.stepMode = stepMode;
                return layout;
            }
        };

        Mesh(WGPUDevice device, const std::vector<Vertex> &vertices, std::vector<uint32_t> indices);
        WebGPUVertexBuffer<Vertex> *VertexBuffer()
        { return m_VertexBuffer; }
        WebGPUIndexBuffer<uint32_t> *IndexBuffer()
        { return m_IndexBuffer; }
        WebGPUUniformBuffer<glm::mat4> *UniformBuffer()
        { return m_UniformBuffer; }
        uint32_t IndexCount()
        { return m_Indices.size(); }
        uint32_t VertexCount()
        { return m_Vertices.size(); }
    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
        WebGPUVertexBuffer<Vertex> *m_VertexBuffer;
        WebGPUIndexBuffer<uint32_t> *m_IndexBuffer;
        WebGPUUniformBuffer<glm::mat4> *m_UniformBuffer;
    };
}
