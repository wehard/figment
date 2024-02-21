#pragma once

#include "WebGPUContext.h"
#include "Camera.h"
#include "WebGPUBuffer.h"
#include "BindGroup.h"
#include "RenderPipeline.h"
#include "Mesh.h"

namespace Figment
{
    class MeshRenderer
    {
    public:
        struct Vertex
        {
            glm::vec3 Position;
        };
        MeshRenderer(WebGPUContext &context);
        ~MeshRenderer() = default;
        void BeginFrame(Camera &camera);
        void EndFrame();
        void Draw(Mesh &mesh, glm::mat4 transform);
    private:
        struct CameraData
        {
            glm::mat4 ViewMatrix;
            glm::mat4 ProjectionMatrix;
        };

        struct MeshData
        {
            glm::mat4 ModelMatrix;
        };

        void CreateDefaultPipeline(WebGPUShader &shader, WGPUVertexBufferLayout &vertexBufferLayout);

        WebGPUContext &m_Context;
        BindGroup *m_BindGroup = nullptr;
        RenderPipeline *m_RenderPipeline = nullptr;
        WGPUCommandEncoder m_CommandEncoder = nullptr;
        WebGPUUniformBuffer<CameraData> *m_CameraDataUniformBuffer;
        WebGPUUniformBuffer<MeshData> *m_MeshDataUniformBuffer;
        WebGPUShader *m_DefaultShader;

        WebGPUTexture *m_DepthTexture;
        WGPURenderPassEncoder m_RenderPassEncoder;
    };
}
