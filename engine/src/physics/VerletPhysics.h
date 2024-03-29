#pragma once

#include <vector>

namespace Figment
{
    class Entity;

    class VerletPhysics
    {
    private:
        float m_WorldWidth = 40;
        float m_WorldHeight = 20;
        const float m_BounceFriction = 0.9;
        const float m_Gravity = 0.1;
        const float m_Friction = 0.999;

    public:
        VerletPhysics(/* args */);
        ~VerletPhysics();
        void Update(Entity &entity, std::vector<Entity> others, float deltaTime);
        float GetWidth()
        { return m_WorldWidth; }
        float GetHeight()
        { return m_WorldHeight; }
    };
}
