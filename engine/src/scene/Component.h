#pragma once

#include "FigmentComponent.h"
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

    struct AnimateComponent
    {
    public:
        enum class MotionType
        {
            PingPong,
            Loop,
            Once,
            OnceAndBack,
        };

        MotionType Type = MotionType::OnceAndBack;
        float Speed = 1.0f;
        float Min = 0.0f;
        float Max = 1.0f;

        AnimateComponent() = default;
        AnimateComponent(const AnimateComponent &) = default;

        void Update(float deltaTime)
        {
            if (m_Value == nullptr)
                return;

            switch (Type)
            {
            case MotionType::PingPong:
                UpdatePingPong(deltaTime);
                break;
            case MotionType::Loop:
                UpdateLoop(deltaTime);
                break;
            case MotionType::Once:
                UpdateOnce(deltaTime);
                break;
            case MotionType::OnceAndBack:
                UpdateOnceAndBack(deltaTime);
                break;
            }

            *m_Value += Speed * deltaTime;
        }

        void Set(float *value)
        {
            m_Value = value;
        }

        static std::vector<const char *> GetMotionTypeLabels()
        {
            return {
                    "PingPong",
                    "Loop",
                    "Once",
                    "OnceAndBack"
            };
        }

    private:
        float *m_Value = nullptr;

        void UpdatePingPong(float deltaTime)
        {
            if (*m_Value >= Max)
            {
                Speed = -Speed;
                *m_Value = Max;
            }
            else if (*m_Value <= Min)
            {
                Speed = -Speed;
                *m_Value = Min;
            }
        }

        void UpdateLoop(float deltaTime)
        {
            if (*m_Value >= Max)
                *m_Value = Min;
            else if (*m_Value <= Min)
                *m_Value = Max;
        }

        void UpdateOnce(float deltaTime)
        {
            if (*m_Value >= Max)
                *m_Value = Max;
            else if (*m_Value <= Min)
                *m_Value = Min;
        }

        void UpdateOnceAndBack(float deltaTime)
        {
            if (*m_Value >= Max)
            {
                Speed = -Speed;
                *m_Value = Max;
            }
            else if (*m_Value <= Min)
            {
                Speed = -Speed;
                *m_Value = Min;
            }
        }
    };
}
