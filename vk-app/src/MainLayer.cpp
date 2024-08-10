#include "MainLayer.h"
#include "imgui_impl_vulkan.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/euler_angles.hpp"
#include "Input.h"
#include "Window.h"

MainLayer::MainLayer(const VulkanContext &context, GLFWwindow *window)
        : Layer("MainLayer"), m_Context(context),
        m_Renderer(context),
        m_Camera(1280.0 / 720.0)
{
    m_Renderer.InitGui(window);

    m_Image = Image::Load("res/texture.png");

    m_Texture = new VulkanTexture(m_Context, {
            .Width = static_cast<int>(m_Image.GetWidth()),
            .Height = static_cast<int>(m_Image.GetHeight()),
            .Channels = 4,
            .Data = m_Image.GetData(),
            .Format = VK_FORMAT_R8G8B8A8_UNORM
    });

    std::vector<VulkanContext::Vertex> vertices = {
            {{ -0.5, -0.5, 0.0 }, { 1.0, 0.0, 0.0 }},
            {{ -0.5, 0.5, 0.0 }, { 0.0, 1.0, 0.0 }},
            {{ 0.5, 0.5, 0.0 }, { 0.0, 0.0, 1.0 }},
            {{ -0.5, -0.5, 0.0 }, { 0.0, 0.0, 1.0 }},
            {{ 0.5, 0.5, 0.0 }, { 1.0, 0.0, 0.0 }},
            {{ 0.5, -0.5, 0.0 }, { 0.0, 1.0, 0.0 }}};
    m_Buffer = new VulkanBuffer(m_Context, {
            .Data = vertices.data(),
            .ByteSize = vertices.size() * sizeof(VulkanContext::Vertex),
            .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    });

    m_TextureId = ImGui_ImplVulkan_AddTexture(m_Texture->GetSampler(), m_Texture->GetImageView(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    m_Camera.SetPosition({ 0.0f, 0.0f, 2.0f });
}

void MainLayer::OnAttach()
{
    Log::Info("Attaching MainLayer");
}

void MainLayer::OnDetach()
{
    Log::Info("Detaching MainLayer");
    m_Renderer.Shutdown();
}

void MainLayer::OnEnable()
{
}

void MainLayer::OnDisable()
{

}

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

void MainLayer::OnUpdate(float deltaTime)
{
    Input::Update();
    m_Camera.Update();

    m_Rotation.z += 1.0f;
    m_Position.x = sinf((float)glfwGetTime() * 2.0f); // * 0.5f;
    m_Renderer.BeginFrame();
    m_Renderer.Begin(m_Camera);
    m_Renderer.Draw(*m_Buffer, Transform(
                    m_Position,
                    m_Rotation,
                    { 1.0f, 1.0f, 1.0f }),
            m_Camera);
    m_Renderer.End();

    m_Renderer.BeginGuiPass();
    DrawTestWindow(*m_Texture, m_TextureId, (float)m_Image.GetWidth(), (float)m_Image.GetHeight());
    ImGui::Begin("Camera");
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", m_Camera.GetPosition().x, m_Camera.GetPosition().y,
            m_Camera.GetPosition().z);
    ImGui::End();
    m_Renderer.EndGuiPass();

    m_Renderer.EndFrame();
}

void MainLayer::OnImGuiRender()
{

}

void MainLayer::OnEvent(AppEvent event, void *eventData)
{
    if (event != AppEvent::WindowResize)
        return;
    auto resizeEvent = (Figment::WindowResizeEventData *)eventData;
    m_Camera.Resize((float)resizeEvent->Width, (float)resizeEvent->Height);
    m_Renderer.OnResize(resizeEvent->Width, resizeEvent->Height);
}
