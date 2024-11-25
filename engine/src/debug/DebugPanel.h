#pragma once
#include "Camera.h"
#include "Entity.h"
#include "Window.h"

namespace Figment
{
    void DrawDebugPanel(const Window& window, Camera &camera, bool collapsed = false);
}
