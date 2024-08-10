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
    Application app({ .Name = "Figment", .Width = 1280, .Height = 720 });
    auto window = app.GetWindow();
    auto nativeWindow = (GLFWwindow *)window.GetNative();
    auto context = window.GetContext<VulkanContext>();
    app.AddLayer(new MainLayer(*context, (GLFWwindow *)window.GetNative()));

    while (!window.ShouldClose() && glfwGetKey(nativeWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        app.Update();
    }
    return 0;
}
