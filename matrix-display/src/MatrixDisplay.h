#pragma once

#include "Layer.h"
#include "Core.h"
#include "Scene.h"

class MatrixDisplay : public Figment::Layer
{
public:
    MatrixDisplay(float windowWidth, float windowHeight);
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(Figment::AppEvent event, void *eventData) override;
    void PutPixel(uint32_t x, uint32_t y, glm::vec4 color);
    void Clear();
    void Fill(glm::vec4 color);
private:
    static constexpr glm::vec4 m_BackgroundColor = glm::vec4(0.1, 0.1, 0.1, 1.0);
    static constexpr uint32_t m_Width = 64;
    static constexpr uint32_t m_Height = 32;
    Figment::UniquePtr<Figment::Scene> m_Scene;
    glm::vec4 m_Matrix[m_Width * m_Height];
    Figment::UniquePtr<Figment::WebGPURenderer> m_Renderer;
    Figment::UniquePtr<Figment::OrthographicCamera> m_Camera;
    glm::vec2 m_MousePosition;
    glm::vec2 m_MousePositionWorldSpace;
    glm::vec2 m_WindowSize;
    glm::vec4 m_DrawColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
};
