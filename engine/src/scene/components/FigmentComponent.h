#pragma once

#include "Utils.h"
#include "WebGPUShader.h"
#include "WebGPUBuffer.h"
#include <string>

namespace Figment
{
    struct FigmentComponent
    {
    public:
        struct FigmentData
        {
            float Time = 0.0;
            int32_t Id = -1;
            uint32_t _Padding[2]; // WGSL requires alignment: https://gpuweb.github.io/gpuweb/wgsl/#alignment-and-size https://www.w3.org/TR/WGSL/#alignment-and-size
            glm::mat4 Model = glm::mat4(1.0);
        };

        constexpr static uint32_t MaxShaderSourceSize = 4096;

        struct FigmentConfig
        {
            int Width = 32;
            int Height = 32;
            char ComputeShaderSourceBuffer[MaxShaderSourceSize] = "";
            char ShaderSourceBuffer[MaxShaderSourceSize] = "";
            glm::vec4 Color = glm::vec4(1.0);
            bool DrawPoints = false;

            int Count() const
            {
                return Width * Height;
            }
        };

        struct Vertex // TODO: Verify alignment
        {
            glm::vec3 Position;
            uint32_t _Padding[1];
            glm::vec3 Normal;
            uint32_t _Padding2[1];
            glm::vec2 TexCoord;
            uint32_t _Padding3[2];
            glm::vec4 Color;

            static uint32_t Size()
            {
                return sizeof(Vertex);
            }

            static std::vector<WGPUVertexAttribute> Layout()
            {
                return std::vector<WGPUVertexAttribute>({
                        {
                                .format = WGPUVertexFormat_Float32x3,
                                .offset = 0,
                                .shaderLocation = 0,
                        },
                        {
                                .format = WGPUVertexFormat_Float32x3,
                                .offset = 16,
                                .shaderLocation = 1,
                        },
                        {
                                .format = WGPUVertexFormat_Float32x2,
                                .offset = 32,
                                .shaderLocation = 2,
                        },
                        {
                                .format = WGPUVertexFormat_Float32x4,
                                .offset = 48,
                                .shaderLocation = 3,
                        },
                });
            }
        };

        FigmentConfig Config;
        std::shared_ptr<WebGPUShader> Shader = nullptr;
        std::shared_ptr<WebGPUShader> ComputeShader = nullptr;
        std::shared_ptr<WebGPUUniformBuffer<FigmentData>> UniformBuffer = nullptr;
        std::shared_ptr<WebGPUIndexBuffer<uint32_t>> IndexBuffer = nullptr;
        std::shared_ptr<WebGPUVertexBuffer<Vertex>> VertexBuffer = nullptr;

        bool Initialized = false;

        FigmentComponent() = default;
        explicit FigmentComponent(WGPUDevice
        device) :
                m_Device(device)
        {
            auto computeShaderSource = Utils::LoadFile("res/shaders/compute.wgsl");
            std::copy(computeShaderSource.begin(), computeShaderSource.end(), Config.ComputeShaderSourceBuffer);

            auto shaderSource = Utils::LoadFile("res/shaders/figment.wgsl");
            std::copy(shaderSource.begin(), shaderSource.end(), Config.ShaderSourceBuffer);

            CreateBuffers();
            Init();
        }

        ~FigmentComponent()
        {
        }

        void Init()
        {
            if (m_Device == nullptr)
            {
                FIG_LOG_ERROR("FigmentComponent::Init: m_Device is null");
                return;
            }

            Shader = std::make_shared<WebGPUShader>(m_Device, Config.ShaderSourceBuffer, "FigmentShader");
            ComputeShader = std::make_shared<WebGPUShader>(m_Device, Config.ComputeShaderSourceBuffer,
                    "ComputeShader");

            Initialized = true;
        }

        void CreateBuffers()
        {
            uint64_t vertexBufferSize = Config.Count() * Vertex::Size();
            uint64_t indexBufferSize = Config.Count() * 6 * sizeof(uint32_t);

            UniformBuffer = std::make_shared<WebGPUUniformBuffer<FigmentData>>
                    (m_Device, "FigmentDataUniformBuffer",
                            sizeof(FigmentData));
            IndexBuffer = std::make_shared<WebGPUIndexBuffer<uint32_t>>
                    (m_Device, "FigmentIndexBuffer",
                            indexBufferSize);
            VertexBuffer = std::make_shared<WebGPUVertexBuffer<Vertex>>
                    (m_Device, "FigmentVertexBuffer",
                            vertexBufferSize);
            VertexBuffer->SetVertexLayout(Vertex::Layout(), Vertex::Size(), WGPUVertexStepMode_Vertex);
        }

    private:
        WGPUDevice m_Device;
    };
}
