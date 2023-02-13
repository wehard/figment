#pragma once

#include "OpenGLBuffer.h"
#include "glm/glm.hpp"

class Transform
{
private:
    glm::vec3 m_Position;
    glm::vec3 m_Scale;
    glm::vec3 m_Rotation;

public:
    Transform();
    ~Transform();
};

Transform::Transform() : m_Position(glm::vec3(0)), m_Rotation(glm::vec3(0)), m_Scale(glm::vec3(1))
{
}

Transform::~Transform()
{
}

class Entity
{
private:
    OpenGLVertexBuffer *m_VertexBuffer;
    OpenGLIndexBuffer *m_IndexBuffer;
    Transform m_Transform;

public:
    Entity();
    ~Entity();
};

Entity::Entity()
{
}

Entity::~Entity()
{
}
