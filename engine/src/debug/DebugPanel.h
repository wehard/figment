#pragma once
#include "BaseWindow.h"
#include "Camera.h"
#include "Entity.h"

#include <FPSCounter.h>

namespace figment
{
void DrawDebugPanel(const Window& window, Camera& camera, const FPSCounter& fpsCounter,
                    bool collapsed = false);
}
