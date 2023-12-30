#include "MatrixDisplay.h"
#include "App.h"
#include "WebGPUWindow.h"

MatrixDisplay::MatrixDisplay(float windowWidth, float windowHeight)
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Renderer = Figment::CreateUniquePtr<Figment::WebGPURenderer>(*webGpuWindow->GetContext());
    m_Camera = Figment::CreateUniquePtr<Figment::PerspectiveCamera>(windowWidth / windowHeight);
    m_Camera->GetPositionPtr()->z = 50.0;

    Clear();
    PutPixel(10, 10, glm::vec4(1.0, 0.0, 0.0, 1.0));
}

void MatrixDisplay::OnAttach()
{

}

void MatrixDisplay::OnDetach()
{

}

void MatrixDisplay::OnUpdate(float deltaTime)
{
    m_Camera->Update();
    m_Renderer->Begin(*m_Camera);
    for (int y = 0; y < m_Height; ++y)
    {
        for (int x = 0; x < m_Width; ++x)
        {
            glm::vec4 color = m_Matrix[y * m_Width + x];
            m_Renderer->SubmitCircle(glm::vec3(x - (float)(m_Width / 2), y - (float)(m_Height / 2), 0), glm::vec3(1.0), color, 1);
        }
    }
    m_Renderer->End();
}

void MatrixDisplay::OnImGuiRender()
{
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
    ImGui::Begin("Matrix Display");
    if (ImGui::Button("Clear"))
        Clear();
    ImGui::End();
}

void MatrixDisplay::OnEvent(Figment::AppEvent event, void *eventData)
{

}

void MatrixDisplay::PutPixel(uint32_t x, uint32_t y, glm::vec4 color)
{
    if (x >= m_Width || y >= m_Height)
    {
        return;
    }
    m_Matrix[y * m_Width + x] = color;
}

void MatrixDisplay::Clear()
{
    for (auto & i : m_Matrix)
    {
        i = m_BackgroundColor;
    }
}
