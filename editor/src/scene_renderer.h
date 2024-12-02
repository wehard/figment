#pragma once

#include "Camera.h"

namespace figment
{
class SceneRenderer
{
public:
    SceneRenderer();
    ~SceneRenderer();

    void begin(const Camera& camera);
    void end();

    void draw(const Entity& entity);

private:
};

} // namespace figment
