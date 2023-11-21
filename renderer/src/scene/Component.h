#pragma once

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

    struct FigmentData
    {
        float Time = 0.0;
    };

    struct FigmentComponent
    {
        constexpr static uint32_t MaxShaderSourceSize = 4096;
        struct FigmentConfig
        {
            int Count = 64;
            char ComputeShaderSourceBuffer[MaxShaderSourceSize] = "";
        };

        FigmentConfig Config;
        WebGPUShader *Shader = nullptr;
        WebGPUShader *ComputeShader = nullptr;
        WebGPUUniformBuffer<FigmentData> *UniformBuffer = nullptr;
        WebGPUBuffer<glm::vec4> *Result = nullptr;
        WebGPUBuffer<glm::vec4> *MapBuffer = nullptr;
        glm::vec4 *Data = nullptr;
        glm::vec4 Color = glm::vec4(1.0);
        bool Initialized = false;

        FigmentComponent() = default;

        ~FigmentComponent()
        {
            delete Result;
            delete MapBuffer;
            delete Data;
            delete UniformBuffer;
        }

        void Init(WGPUDevice device)
        {
            uint64_t size = Config.Count * sizeof(glm::vec4);

            auto computeShaderSource = Utils::LoadFile2("res/shaders/wgsl/compute.wgsl");
            std::copy(computeShaderSource->begin(), computeShaderSource->end(), Config.ComputeShaderSourceBuffer);

            Shader = new WebGPUShader(device, *Utils::LoadFile2("res/shaders/wgsl/figment.wgsl"), "FigmentShader");
            ComputeShader = new WebGPUShader(device, *computeShaderSource, "ComputeShader");
            Result = new WebGPUBuffer<glm::vec4>(device, "FigmentBuffer", size,
                    WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
            MapBuffer = new WebGPUBuffer<glm::vec4>(device, "FigmentMapBuffer", size,
                    WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead);
            UniformBuffer = new WebGPUUniformBuffer<FigmentData>(device, "FigmentData", sizeof(FigmentData));
            Initialized = true;
        }

        void UpdateComputeShader(WGPUDevice device)
        {
            if (Initialized)
            {
                delete ComputeShader;
                ComputeShader = new WebGPUShader(device, Config.ComputeShaderSourceBuffer, "ComputeShader");
            }
        }
    };

    struct CameraComponent
    {
    public:
        std::shared_ptr<CameraController> Controller;

        CameraComponent()
        {
            Camera = std::make_shared<PerspectiveCamera>(1.777f);
            Controller = std::make_shared<CameraController>(Camera);
        }
        CameraComponent(const CameraComponent &) = default;
        explicit CameraComponent(std::shared_ptr<PerspectiveCamera> camera) : Camera(camera)
        {
            Controller = std::make_shared<CameraController>(Camera);
        }
    private:
        std::shared_ptr<PerspectiveCamera> Camera;
    };
}
