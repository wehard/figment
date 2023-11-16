#pragma once

#include "UUID.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <string>

struct IdComponent
{
    Figment::UUID UUID;

    IdComponent() = default;
    IdComponent(const IdComponent &) = default;
    explicit IdComponent(Figment::UUID uuid) : UUID(uuid) {}
};

struct InfoComponent
{
    std::string m_Name;

    InfoComponent() = default;
    InfoComponent(const InfoComponent &) = default;
    InfoComponent(const std::string &name) : m_Name(name) {}
};

struct TransformComponent
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

    TransformComponent() = default;
    TransformComponent(const TransformComponent &) = default;
    TransformComponent(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale) : Position(position), Rotation(rotation), Scale(scale) {}

    // operator glm::mat4 &() { return m_Transform; }
    glm::mat4 GetTransform()
    {
        glm::mat4 matScale = glm::scale(glm::mat4(1.0f), Scale);
        glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), Position);
        glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(Rotation.x), glm::radians(Rotation.y), glm::radians(Rotation.z));
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
    CircleComponent(float radius) : Radius(radius) {}
};

struct QuadComponent
{
    glm::vec4 Color = glm::vec4(1.0);

    QuadComponent() = default;
};

struct FigmentComponent
{
    WebGPUShader &Shader;
    WebGPUShader &ComputeShader;
    WebGPUBuffer<glm::vec4> *Buffer = nullptr;
    WebGPUBuffer<glm::vec4> *MapBuffer = nullptr;
    glm::vec4 *Data = nullptr;
    glm::vec4 Color = glm::vec4(1.0);

    FigmentComponent(WebGPUShader &shader, WebGPUShader &computeShader) : Shader(shader), ComputeShader(computeShader) {}
    ~FigmentComponent()
    {
        delete Buffer;
        delete MapBuffer;
        delete Data;
    }
};
