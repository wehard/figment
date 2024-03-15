#include "GameOfLife.h"
#include "ComputePass.h"

GameOfLife::GameOfLife(Figment::WebGPUContext &context, PerspectiveCamera &camera) : Layer("GameOfLife"),
        m_Context(context), m_Camera(camera)
{
    PixelCanvasDescriptor descriptor = { s_Width, s_Height, false };
    m_PixelCanvas = new PixelCanvas(context, &descriptor);

    m_PixelCanvas->Fill(s_DeadColor);

    for (auto x = 0; x < s_Width * s_Height; x++)
    {
        auto i = x % s_Width;
        auto j = x / s_Width;
        auto r = Random::Float();
        if (r < 0.2)
            m_PixelCanvas->SetPixel(i, j, s_LiveColor);
    }
    m_PixelCanvas->UpdateTexture();

    m_UniformBuffer = new WebGPUUniformBuffer<TextureData>(context.GetDevice(), "TextureDataUniformBuffer",
            sizeof(TextureData));
    TextureData data = { s_Width, s_Height };
    m_UniformBuffer->SetData(&data, sizeof(TextureData));

    m_PrevPixelData = new uint32_t[s_Width * s_Height];
    memcpy(m_PrevPixelData, m_PixelCanvas->GetPixelData(), s_Width * s_Height * sizeof(uint32_t));
}

void GameOfLife::OnAttach()
{

}

void GameOfLife::OnDetach()
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

    m_PixelCanvas->OnUpdate(m_Camera, deltaTime);
}

void GameOfLife::OnImGuiRender()
{

}

void GameOfLife::OnEvent(Figment::AppEvent event, void *eventData)
{

}
