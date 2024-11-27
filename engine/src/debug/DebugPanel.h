#pragma once
#include "Camera.h"
#include "Entity.h"
#include "BaseWindow.h"

namespace figment
{
void DrawDebugPanel(const Window& window, Camera& camera, bool collapsed = false);
}
