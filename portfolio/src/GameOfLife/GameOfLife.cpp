#include "GameOfLife.h"
#include "ComputePass.h"

GameOfLife::GameOfLife(Figment::WebGPUContext &context, PerspectiveCamera &camera) : Layer("GameOfLife"),
        m_Context(context), m_MeshRenderer(context), m_Camera(camera)
{
    m_PixelCanvas = new PixelCanvas(context, { s_Width, s_Height });

    m_PrevPixelData = new uint32_t[s_Width * s_Height];
    memcpy(m_PrevPixelData, m_PixelCanvas->GetPixelData(), s_Width * s_Height * sizeof(uint32_t));

    Randomize();
}

void GameOfLife::OnAttach()
{

}

void GameOfLife::OnDetach()
{

}

void GameOfLife::OnEnable()
{
    m_Camera.SetPosition(glm::vec3(0.0, 0.0, 2.0));
}

void GameOfLife::OnDisable()
{

}

static uint32_t GetLiveNeighbours(uint32_t *canvas, int w, int h, int cx, int cy, uint32_t liveColor,
        uint32_t deadColor)
{
    uint32_t count = 0;

    for (int y = -1; y < 2; y++)
    {
        for (int x = -1; x < 2; x++)
        {
            int sx = cx + x;
            int sy = cy + y;
            if ((x == 0 && y == 0) || sx < 0 || sx > w - 1 || sy < 0 || sy > h - 1)
                continue;
            auto c = canvas[sy * w + sx];
            if (c == liveColor)
            {
                count++;
            }
        }
    }
    return count;
}

static glm::mat4 Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
    glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y),
            glm::radians(rotation.z));
    return matTranslate * matRotate * matScale;
}

static float counter = 0.0f;

void GameOfLife::OnUpdate(float deltaTime)
{
    counter += deltaTime;
    if (counter > 0.05f)
    {
        for (int y = 0; y < s_Height; y++)
        {
            for (int x = 0; x < s_Width; x++)
            {
                auto pixel = m_PrevPixelData[y * s_Width + x];
                auto liveNeighbours = GetLiveNeighbours(m_PrevPixelData, s_Width, s_Height, x, y, s_LiveColor,
                        s_DeadColor);
                if (pixel == s_LiveColor)
                {
                    if (liveNeighbours < 2 || liveNeighbours > 3)
                    {
                        m_PixelCanvas->SetPixel(x, y, s_DeadColor);
                    }
                }
                else if (pixel == s_DeadColor)
                {
                    if (liveNeighbours == 3)
                    {
                        m_PixelCanvas->SetPixel(x, y, s_LiveColor);
                    }
                }
            }
        }
        m_PixelCanvas->UpdateTexture();

        memcpy(m_PrevPixelData, m_PixelCanvas->GetPixelData(), s_Width * s_Height * sizeof(uint32_t));

        counter = 0.0f;
    }

    auto time = glfwGetTime();
    m_Rotation.y = cos(time * 0.25f) * 15.0f;
    m_Rotation.z = sin(time * 0.25f) * 15.0f;

    m_MeshRenderer.BeginFrame(m_Camera);
    m_MeshRenderer.DrawTextured(m_PixelCanvas->GetMesh(), Transform(m_Position, m_Rotation, m_Scale),
            m_PixelCanvas->GetTexture());
    m_MeshRenderer.EndFrame();
}

void GameOfLife::OnImGuiRender()
{

}

void GameOfLife::OnEvent(Figment::AppEvent event, void *eventData)
{
    if (event == Figment::AppEvent::WindowResize)
    {
        auto ev = (Figment::WindowResizeEventData *)eventData;
        m_MeshRenderer.OnResize(ev->Width, ev->Height);
    }
}

void GameOfLife::Randomize()
{
    m_PixelCanvas->Fill(s_DeadColor);
    for (auto y = 0; y < s_Height; y++)
    {
        for (auto x = 0; x < s_Width; x++)
        {
            if (Random::Float() < 0.5)
                m_PixelCanvas->SetPixel(x, y, s_LiveColor);
        }
    }
    m_PixelCanvas->UpdateTexture();
    memcpy(m_PrevPixelData, m_PixelCanvas->GetPixelData(), s_Width * s_Height * sizeof(uint32_t));
}
