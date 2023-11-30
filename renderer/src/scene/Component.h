#pragma once

#include "Core.h"
#include "UUID.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include "CameraController.h"
#include "Utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <string>

namespace Figment
{
    struct IdComponent
    {
        Figment::UUID UUID;

        IdComponent() = default;
        IdComponent(const IdComponent &) = default;
        explicit IdComponent(Figment::UUID uuid) : UUID(uuid)
        { }
    };

    struct InfoComponent
    {
        std::string m_Name;

        InfoComponent() = default;
        InfoComponent(const InfoComponent &) = default;
        InfoComponent(const std::string &name) : m_Name(name)
        { }
    };

    struct TransformComponent
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent &) = default;
        TransformComponent(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale) : Position(
                position), Rotation(rotation), Scale(scale)
        { }

        // operator glm::mat4 &() { return m_Transform; }
        glm::mat4 GetTransform()
        {
            glm::mat4 matScale = glm::scale(glm::mat4(1.0f), Scale);
            glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), Position);
            glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(Rotation.x), glm::radians(Rotation.y),
                    glm::radians(Rotation.z));
            glm::mat4 m = matTranslate * matRotate * matScale;
            return (m);
        }
    };

    struct VerletBodyComponent
    {
        glm::vec3 m_PreviousPosition = glm::vec3(0);
        glm::vec3 m_Velocity = glm::vec3(0.0, 0.0, 0.0);
    };

    struct CircleComponent
    {
        glm::vec4 Color = glm::vec4(1.0);
        float Thickness = 0.1f;
        float Radius = 1.0f;

        CircleComponent() = default;
        CircleComponent(float radius) : Radius(radius)
        { }
    };

    struct QuadComponent
    {
        glm::vec4 Color = glm::vec4(1.0);

        QuadComponent() = default;
    };

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
            int Count = 16384;
            char ComputeShaderSourceBuffer[MaxShaderSourceSize] = "";
            char ShaderSourceBuffer[MaxShaderSourceSize] = "";
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
        SharedPtr<WebGPUShader> Shader = nullptr;
        SharedPtr<WebGPUShader> ComputeShader = nullptr;
        SharedPtr<WebGPUUniformBuffer<FigmentData>> UniformBuffer = nullptr;
        SharedPtr<WebGPUIndexBuffer<uint32_t>> IndexBuffer = nullptr;
        SharedPtr<WebGPUVertexBuffer<Vertex>> VertexBuffer = nullptr;

        glm::vec4 *Data = nullptr;
        glm::vec4 Color = glm::vec4(1.0);
        bool Initialized = false;

        FigmentComponent() = default;
        explicit FigmentComponent(WGPUDevice
        device) :
                m_Device(device)
        {
            auto computeShaderSource = Utils::LoadFile2("res/shaders/wgsl/compute.wgsl");
            std::copy(computeShaderSource->begin(), computeShaderSource->end(), Config.ComputeShaderSourceBuffer);

            auto shaderSource = Utils::LoadFile2("res/shaders/wgsl/figment.wgsl");
            std::copy(shaderSource->begin(), shaderSource->end(), Config.ShaderSourceBuffer);

            CreateBuffers();
            Init();
        }

        ~FigmentComponent()
        {
            delete Data;
        }

        void Init()
        {
            if (m_Device == nullptr)
            {
                printf("FigmentComponent::Init: m_Device is null\n");
                return;
            }

            Shader = CreateSharedPtr<WebGPUShader>(m_Device, Config.ShaderSourceBuffer, "FigmentShader");
            ComputeShader = CreateSharedPtr<WebGPUShader>(m_Device, Config.ComputeShaderSourceBuffer,
                    "ComputeShader");

            Initialized = true;
        }

        void CreateBuffers()
        {
            uint64_t vertexBufferSize = Config.Count * Vertex::Size();
            uint64_t indexBufferSize = Config.Count * 6 * sizeof(uint32_t);

            UniformBuffer = CreateSharedPtr<WebGPUUniformBuffer<FigmentData>>(m_Device, "FigmentDataUniformBuffer",
                    sizeof(FigmentData));
            IndexBuffer = CreateSharedPtr<WebGPUIndexBuffer<uint32_t>>(m_Device, "FigmentIndexBuffer",
                    indexBufferSize);
            VertexBuffer = CreateSharedPtr<WebGPUVertexBuffer<Vertex>>(m_Device, "FigmentVertexBuffer",
                    vertexBufferSize);
            VertexBuffer->SetVertexLayout(Vertex::Layout(), Vertex::Size(), WGPUVertexStepMode_Vertex);
        }

    private:
        WGPUDevice m_Device;
    };

    struct CameraComponent
    {
    public:
        SharedPtr<CameraController> Controller;

        CameraComponent()
        {
            Camera = CreateSharedPtr<PerspectiveCamera>(1.777f);
            Controller = CreateSharedPtr<CameraController>(Camera);
        }
        CameraComponent(const CameraComponent &) = default;
        explicit CameraComponent(SharedPtr<PerspectiveCamera> camera) : Camera(camera)
        {
            Controller = CreateSharedPtr<CameraController>(Camera);
        }
    private:
        SharedPtr<PerspectiveCamera> Camera;
    };
}
