#include "Application.h"
#include "Input.h"
#include "PerspectiveCamera.h"
#include "Image.h"

#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "Renderer.h"

#include "glm/glm.hpp"
#include "MainLayer.h"

using namespace Figment;

int main()
{
    Application app({ .Name = "Figment (vk-app)", .Width = 1280, .Height = 720 });
    auto window = app.GetWindow();
    auto context = window.GetContext<VulkanContext>();
    app.AddLayer(new MainLayer(*context, (GLFWwindow *)window.GetNative()));
    app.Start();
    return 0;
}
