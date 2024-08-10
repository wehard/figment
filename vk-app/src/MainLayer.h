#pragma once

#include "VulkanContext.h"
#include "Renderer.h"
#include "VulkanTexture.h"
#include "Layer.h"
#include "PerspectiveCamera.h"
#include "Image.h"

using namespace Figment;

class MainLayer : public Layer
{
public:
    explicit MainLayer(const VulkanContext &context, GLFWwindow *window);

    void OnAttach() override;
    void OnDetach() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;
private:
    const VulkanContext &m_Context;
    Vulkan::Renderer m_Renderer;
    PerspectiveCamera m_Camera;

    Image m_Image;
    VulkanBuffer *m_Buffer;
    VulkanTexture *m_Texture;
    ImTextureID m_TextureId;

    glm::vec3 m_Position = {};
    glm::vec3 m_Rotation = {};
};
