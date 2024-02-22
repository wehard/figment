#pragma once

#include "WebGPUContext.h"
#include "Camera.h"
#include "WebGPUBuffer.h"
#include "BindGroup.h"
#include "RenderPipeline.h"
#include "Mesh.h"

#include <unordered_map>

namespace Figment
{
    class MeshRenderer
    {
    public:
        MeshRenderer(WebGPUContext &context);
        ~MeshRenderer() = default;
        void BeginFrame(Camera &camera);
        void EndFrame();
        void Draw(Mesh &mesh, glm::mat4 transform);
        void OnResize(uint32_t width, uint32_t height);
    private:
        struct CameraData
        {
            glm::mat4 ViewMatrix;
            glm::mat4 ProjectionMatrix;
        };

        struct MeshInstanceData
        {
            glm::mat4 ModelMatrix;
        };

        constexpr static uint32_t MaxInstances = 10000;

        struct MeshRenderData
        {
            uint32_t InstanceCount = 0;
            RenderPipeline *Pipeline;
            BindGroup *BindGroup;
            WebGPUVertexBuffer<MeshInstanceData> *InstanceBuffer;
            MeshInstanceData *InstanceDataStagingBuffer;
        };
        std::unordered_map<Mesh *, MeshRenderData> m_MeshRenderData;

        WebGPUContext &m_Context;
        WGPUCommandEncoder m_CommandEncoder = nullptr;
        WebGPUUniformBuffer<CameraData> *m_CameraDataUniformBuffer;
        WebGPUShader *m_DefaultShader;

        WebGPUTexture *m_DepthTexture;
        WGPURenderPassEncoder m_RenderPassEncoder;
    };
}
