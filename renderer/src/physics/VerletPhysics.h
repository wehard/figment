#pragma once

class Entity;

class VerletPhysics
{
private:
    float m_WorldWidth = 20;
    float m_WorldHeight = 20;
    float m_BounceFriction = 0.99;
    const float m_Gravity = 0.1;
    const float m_Friction = 0.9;

public:
    VerletPhysics(/* args */);
    ~VerletPhysics();
    void Update(Entity &entity, float deltaTime);
};
