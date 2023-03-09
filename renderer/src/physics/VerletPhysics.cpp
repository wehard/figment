#include "VerletPhysics.h"
#include "Entity.h"

#include <glm/glm.hpp>

VerletPhysics::VerletPhysics()
{
}

VerletPhysics::~VerletPhysics()
{
}

void VerletPhysics::Update(Entity &entity, std::vector<Entity> others, float deltaTime)
{
    auto &transform = entity.GetComponent<TransformComponent>();
    auto &body = entity.GetComponent<VerletBodyComponent>();

    float vx = (transform.Position.x - body.m_PreviousPosition.x) * m_Friction;
    float vy = (transform.Position.y - body.m_PreviousPosition.y) * m_Friction;

    body.m_PreviousPosition.x = transform.Position.x;
    body.m_PreviousPosition.y = transform.Position.y;

    transform.Position.x += vx;
    transform.Position.y += vy;
    transform.Position.y -= m_Gravity;

    // for (Entity other : others)
    // {
    //     if (other == entity)
    //         continue;
    //     auto ot = other.GetComponent<TransformComponent>();
    //     glm::vec3 dir = ot.Position - transform.Position;
    //     float dist = glm::length(dir);
    //     if (dist < 1.0)
    //     {
    //         glm::normalize(dir);
    //         transform.Position += -dir * (dist / 2);
    //     }
    // }

    if (transform.Position.x > m_WorldWidth)
    {
        transform.Position.x = m_WorldWidth;
        body.m_PreviousPosition.x = transform.Position.x + vx * m_BounceFriction;
    }
    else if (transform.Position.x < -m_WorldWidth)
    {
        transform.Position.x = -m_WorldWidth;
        body.m_PreviousPosition.x = transform.Position.x + vx * m_BounceFriction;
    }

    if (transform.Position.y < -m_WorldHeight)
    {
        transform.Position.y = -m_WorldHeight;
        body.m_PreviousPosition.y = transform.Position.y + vy * m_BounceFriction;
    }
    else if (transform.Position.y > m_WorldHeight)
    {
        transform.Position.y = m_WorldHeight;
        body.m_PreviousPosition.y = transform.Position.y + vy * m_BounceFriction;
    }

    body.m_Velocity = glm::vec3(vx, vy, 0.0);
}
