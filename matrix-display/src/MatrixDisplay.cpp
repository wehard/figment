#include "MatrixDisplay.h"
#include "App.h"
#include "WebGPUWindow.h"
#include "Input.h"
#include <cmath>

MatrixDisplay::MatrixDisplay(uint32_t width, uint32_t height, float windowWidth, float windowHeight) : m_Width(width), m_Height(height), m_WindowSize(windowWidth, windowHeight)
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Renderer = Figment::CreateUniquePtr<Figment::WebGPURenderer>(*webGpuWindow->GetContext());
    m_Camera = Figment::CreateUniquePtr<Figment::OrthographicCamera>(windowWidth, windowHeight);
    m_Camera->SetPosition(glm::vec3(32, 16, 0.0));
    m_Camera->SetZoom(20.0);

    m_Matrix = new glm::vec4[m_Width * m_Height];

    Clear();
    PutPixel(10, 10, glm::vec4(1.0, 0.0, 0.0, 1.0));
    PutPixel(20, 10, glm::vec4(0.0, 1.0, 0.0, 1.0));
}

MatrixDisplay::~MatrixDisplay()
{
    delete[] m_Matrix;
}

void MatrixDisplay::OnAttach()
{

}

void MatrixDisplay::OnDetach()
{

}

static uint32_t *SerializeMatrix(glm::vec4 *matrix, size_t size)
{
    auto *result = new uint32_t[size];
    for (size_t i = 0; i < size; ++i)
    {
        auto r = (uint32_t)(matrix[i].r * 255.0);
        auto g = (uint32_t)(matrix[i].g * 255.0);
        auto b = (uint32_t)(matrix[i].b * 255.0);
        auto a = (uint32_t)(matrix[i].a * 255.0);
        result[i] = (r << 24) | (g << 16) | (b << 8) | a;
    }
    return result;
}

void MatrixDisplay::OnUpdate(float deltaTime)
{
    if (Figment::Input::GetButtonDown(2))
    {
        m_Camera->BeginPan(m_MousePosition);
    }
    if (Figment::Input::GetButtonUp(2))
    {
        m_Camera->EndPan();
    }

    m_Camera->Update();

    m_MousePosition = Figment::Input::GetMousePosition();
    m_MousePositionWorldSpace = m_Camera->ScreenToWorldSpace(m_MousePosition, m_WindowSize);
    int mx = (int)std::round(m_MousePositionWorldSpace.x);
    int my = (int)std::round(m_MousePositionWorldSpace.y);

    if (Figment::Input::GetButton(0))
    {
        PutPixel(mx, my, m_DrawColor);
    }

    m_Renderer->Begin(*m_Camera);
    for (int y = 0; y < m_Height; ++y)
    {
        for (int x = 0; x < m_Width; ++x)
        {
            glm::vec4 color = m_Matrix[y * m_Width + x];

            if (mx == x && my == y)
            {
                color = m_DrawColor;
            }
            m_Renderer->SubmitCircle(glm::vec3(x, y, 0), glm::vec3(1.0), color, 1);
        }
    }
    m_Renderer->End();

    if (Figment::Input::GetKeyDown(GLFW_KEY_S))
    {
        auto data = SerializeMatrix(m_Matrix, m_Width * m_Height);
        printf("Saving matrix to file\n");
        printf("Matrix size: %u\n", m_Width * m_Height);
    }
}

void MatrixDisplay::OnImGuiRender()
{
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Matrix Display");
    if (ImGui::Button("Fill"))
        Fill(m_DrawColor);
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
        Clear();
    ImGui::Separator();
    ImGui::ColorEdit3("Draw Color", &m_DrawColor.x);
    ImGui::End();
}

void MatrixDisplay::OnEvent(Figment::AppEvent event, void *eventData)
{
    auto ev = (Figment::WindowResizeEventData *)eventData;

    m_WindowSize = glm::vec2(ev->Width, ev->Height);
    m_Camera->Resize((float)ev->Width, (float)ev->Height);
    m_Renderer->OnResize(ev->Width, ev->Height);
}

void MatrixDisplay::PutPixel(uint32_t x, uint32_t y, glm::vec4 color)
{
    if (x >= m_Width || y >= m_Height || m_Matrix == nullptr)
        return;

    m_Matrix[y * m_Width + x] = color;
}

void MatrixDisplay::Clear()
{
    Fill(m_BackgroundColor);
}

void MatrixDisplay::Fill(glm::vec4 color)
{
    if (m_Matrix == nullptr)
        return;

    for (int i = 0; i < m_Width * m_Height; ++i)
        m_Matrix[i] = color;

}

