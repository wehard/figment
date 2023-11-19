#pragma once

#include "UUID.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include "CameraController.h"
#include "Utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <string>

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
    WebGPUShader *Shader;
    WebGPUShader *ComputeShader;
    WebGPUUniformBuffer<FigmentData> *UniformBuffer = nullptr;
    WebGPUBuffer<glm::vec4> *Result = nullptr;
    WebGPUBuffer<glm::vec4> *MapBuffer = nullptr;
    glm::vec4 *Data = nullptr;
    glm::vec4 Color = glm::vec4(1.0);
    bool Initialized = false;

    FigmentComponent()
    {

    }

    FigmentComponent(WebGPUShader *shader, WebGPUShader *computeShader) : Shader(shader), ComputeShader(computeShader)
    {
         Initialized = true;
    }

    ~FigmentComponent()
    {
        delete Result;
        delete MapBuffer;
        delete Data;
    }

    void Init(WGPUDevice device)
    {
        uint64_t count = 64;
        uint64_t size = count * sizeof(glm::vec4);

        Shader = new WebGPUShader(device, *Utils::LoadFile2("res/shaders/wgsl/figment.wgsl"));;
        ComputeShader = new WebGPUShader(device, *Utils::LoadFile2("res/shaders/wgsl/compute.wgsl"));;
        Result = new WebGPUBuffer<glm::vec4>(device, "FigmentBuffer", size,
                WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
        MapBuffer = new WebGPUBuffer<glm::vec4>(device, "FigmentMapBuffer", size,
                WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead);
        UniformBuffer = new WebGPUUniformBuffer<FigmentData>(device, "FigmentData", sizeof(FigmentData));
        Initialized = true;
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
