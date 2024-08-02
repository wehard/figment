#include "Window.h"
#include "Input.h"
#include "PerspectiveCamera.h"
#include "Image.h"

#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "Renderer.h"

#include "imgui_impl_vulkan.h"

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/euler_angles.hpp"

using namespace Figment;

static glm::mat4 Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
    glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y),
            glm::radians(rotation.z));
    return matTranslate * matRotate * matScale;
}

static void DrawTestWindow(VulkanTexture &texture, ImTextureID id, float imageWidth, float imageHeight)
{
    auto pos = Input::GetMousePosition();

    static glm::vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
    ImGui::Begin("Hello, world!");

    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();

    vMin.x += ImGui::GetWindowPos().x;
    vMin.y += ImGui::GetWindowPos().y;
    vMax.x += ImGui::GetWindowPos().x;
    vMax.y += ImGui::GetWindowPos().y;

    auto imagePos = glm::vec2(pos.x - vMin.x, pos.y - vMin.y);

    bool isHovered =
            imagePos.x >= 0 && imagePos.y >= 0 && imagePos.x < imageWidth
                    && imagePos.y < imageHeight;

    if (isHovered && Input::GetButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
        color = texture.GetPixel((int)imagePos.x, (int)imagePos.y);
    }
    ImGui::Image((ImTextureID)id, ImVec2(imageWidth, imageHeight));

    ImGui::ColorEdit4("Color", &color.r);
    ImGui::Text("Screen position: (%.1f, %.1f)", pos.x, pos.y);
    if (isHovered)
        ImGui::Text("Image position: (%.1f, %.1f)", imagePos.x, imagePos.y);

    ImGui::End();
}

int main()
{
    Log::Init();
    auto window = Window::Create("Figment", 1280, 720);
    Input::Initialize((GLFWwindow *)window->GetNative());
    auto vulkanContext = window->GetContext<VulkanContext>();
    auto renderer = Vulkan::Renderer(*vulkanContext);
    renderer.InitGui((GLFWwindow *)window->GetNative());

    PerspectiveCamera camera(1280.0 / 720.0);
    camera.SetPosition({ 0.0f, 0.0f, 2.0f });

    window->SetResizeEventCallback([&camera, &renderer](WindowResizeEventData eventData)
    {
        camera.Resize((float)eventData.Width, (float)eventData.Height);
        renderer.OnResize(eventData.Width, eventData.Height);
    });

    Image image = Image::Load("res/texture.png");

    auto texture = VulkanTexture(*vulkanContext, {
            .Width = static_cast<int>(image.GetWidth()),
            .Height = static_cast<int>(image.GetHeight()),
            .Channels = 4,
            .Data = image.GetData(),
            .Format = VK_FORMAT_R8G8B8A8_UNORM
    });

    std::vector<VulkanContext::Vertex> vertices = {
            {{ -0.5, -0.5, 0.0 }, { 1.0, 0.0, 0.0 }},
            {{ -0.5, 0.5, 0.0 }, { 0.0, 1.0, 0.0 }},
            {{ 0.5, 0.5, 0.0 }, { 0.0, 0.0, 1.0 }},
            {{ -0.5, -0.5, 0.0 }, { 0.0, 0.0, 1.0 }},
            {{ 0.5, 0.5, 0.0 }, { 1.0, 0.0, 0.0 }},
            {{ 0.5, -0.5, 0.0 }, { 0.0, 1.0, 0.0 }}};
    auto buffer = new VulkanBuffer(*vulkanContext, {
            .Data = vertices.data(),
            .ByteSize = vertices.size() * sizeof(VulkanContext::Vertex),
            .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    });

    std::vector<VulkanContext::Vertex> vertices2 = {
            {{ 0.0, 0.5, 0.0 }, { 1.0, 1.0, 0.0 }},
            {{ -0.5, -0.5, 0.0 }, { 1.0, 1.0, 0.0 }},
            {{ 0.5, -0.5, 0.0 }, { 1.0, 1.0, 0.0 }}};

    auto zRotation = 0.0f;
    auto xPosition = 0.0f;

    ImTextureID textureId = ImGui_ImplVulkan_AddTexture(texture.GetSampler(), texture.GetImageView(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    while (!window->ShouldClose() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
    {
        glfwPollEvents();
        Input::Update();
        camera.Update();

        zRotation += 1.0f;
        xPosition = sinf((float)glfwGetTime() * 2.0f); // * 0.5f;
        renderer.BeginFrame();
        renderer.Begin(camera);
        renderer.Draw(*buffer, Transform(
                        { xPosition, 0.0f, 0.0f },
                        { 0.0f, 0.0f, zRotation },
                        { 1.0f, 1.0f, 1.0f }),
                camera);
        renderer.End();

        renderer.BeginGuiPass();
        DrawTestWindow(texture, textureId, (float)image.GetWidth(), (float)image.GetHeight());
        ImGui::Begin("Camera");
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", camera.GetPosition().x, camera.GetPosition().y,
                camera.GetPosition().z);
        ImGui::End();
        renderer.EndGuiPass();

        renderer.EndFrame();
    }
    renderer.Shutdown();

    vulkanContext->Cleanup();
    return 0;
}
