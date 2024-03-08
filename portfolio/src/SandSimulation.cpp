#include "SandSimulation.h"
#include "ComputePass.h"

SandSimulation::SandSimulation(Figment::WebGPUContext &context, PerspectiveCamera &camera) : Layer("SandSimulation"),
        m_Context(context), m_Camera(camera)
{
    PixelCanvasDescriptor descriptor = { s_Width, s_Height, true };
    m_PixelCanvas = new PixelCanvas(context, &descriptor);

    m_PixelCanvas->SetPixel(0, 0, 0xff0000ff);
    m_PixelCanvas->SetPixel(0, 255, 0x00ff00ff);
    m_PixelCanvas->SetPixel(255, 255, 0x0000ffff);
    m_PixelCanvas->SetPixel(255, 0, 0xffffffff);

    for (auto x = 0; x < s_Width * s_Height; x++)
    {
        auto i = x % s_Width;
        auto j = x / s_Width;
        // if ((i / 10 + j / 10) % 2 == 0)
        //     m_PixelCanvas->SetPixel(i, j, 0xff000000);
        // else
        //     m_PixelCanvas->SetPixel(i, j, 0xffffffff);
        auto r = Random::Float();
        if (r < 0.2)
            m_PixelCanvas->SetPixel(i, j, s_SandColor);
        else
            m_PixelCanvas->SetPixel(i, j, s_AirColor);
    }
    m_PixelCanvas->UpdateTexture();

    m_ComputeShader = new WebGPUShader(context.GetDevice(), *Utils::LoadFile2("res/shaders/sand_simulation.wgsl"),
            "SandSimulationCompute");
    m_UniformBuffer = new WebGPUUniformBuffer<TextureData>(context.GetDevice(), "TextureDataUniformBuffer",
            sizeof(TextureData));
    TextureData data = { s_Width, s_Height };
    m_UniformBuffer->SetData(&data, sizeof(TextureData));
}

void SandSimulation::OnAttach()
{

}

void SandSimulation::OnDetach()
{

}

bool SandSimulation::CanMove(PixelCanvas &canvas, int x, int y)
{
    if (x < 0 || x >= canvas.GetWidth() || y < 0 || y >= canvas.GetHeight())
        return false;
    return canvas.GetPixel(x, y) == s_AirColor;
}

void SandSimulation::OnUpdate(float deltaTime)
{
    // auto r = Random::Float();
    // m_PixelCanvas->SetPixel(
    //         ((s_Width / 2) + (uint32_t)(10 * r)) + glm::sin(App::Instance()->GetTimeSinceStart()) * 30,
    //         180,
    //         App::Instance()->GetTimeSinceStart() > 8.0 ? s_WaterColor : s_SandColor);
    //
    // for (int y = 0; y < s_Height; y++)
    // {
    //     for (int x = 0; x < s_Width; x++)
    //     {
    //         auto color = m_PixelCanvas->GetPixel(x, y);
    //         switch (color)
    //         {
    //         case s_SandColor:
    //         {
    //             UpdateSand(x, y);
    //             break;
    //         }
    //         default:
    //             break;
    //         }
    //     }
    // }
    // if (m_Dirty)
    //     m_PixelCanvas->UpdateTexture();

    auto m_ComputeBindGroup = new BindGroup(m_Context.GetDevice(), WGPUShaderStage_Compute);
    m_ComputeBindGroup->Bind(*m_UniformBuffer);
    m_ComputeBindGroup->BindStorage(m_PixelCanvas->GetComputeTexture(), WGPUStorageTextureAccess_ReadOnly);
    m_ComputeBindGroup->BindStorage(m_PixelCanvas->GetTexture(), WGPUStorageTextureAccess_WriteOnly);

    auto m_SimulatePipeline = new ComputePipeline(m_Context.GetDevice(), *m_ComputeBindGroup);
    m_SimulatePipeline->Build("simulate", m_ComputeShader->GetShaderModule());

    ComputePass computePass(m_Context.GetDevice(), m_SimulatePipeline, m_ComputeBindGroup);
    computePass.Begin();
    computePass.Dispatch("simulate", m_PixelCanvas->GetWidth(), m_PixelCanvas->GetHeight());
    computePass.End();

    m_PixelCanvas->OnUpdate(m_Camera, deltaTime);
}

void SandSimulation::OnImGuiRender()
{

}

void SandSimulation::OnEvent(Figment::AppEvent event, void *eventData)
{

}

void SandSimulation::UpdateSand(int x, int y)
{
    if (x < 0 || x >= m_PixelCanvas->GetWidth() || y < 0 || y >= m_PixelCanvas->GetHeight())
        return;
    if (CanMove(*m_PixelCanvas, x, y - 1))
    {
        auto below = m_PixelCanvas->GetPixel(x, y - 1);
        m_PixelCanvas->SetPixel(x, y, below);
        m_PixelCanvas->SetPixel(x, y - 1, s_SandColor);
        m_Dirty = true;
        return;
    }
    bool canMoveDownLeft = CanMove(*m_PixelCanvas, x - 1, y - 1);
    bool canMoveDownRight = CanMove(*m_PixelCanvas, x + 1, y - 1);
    if (canMoveDownLeft && canMoveDownRight)
    {
        auto fc = App::Instance()->GetFPSCounter().GetFrameCount() % 2;
        fc == 0 ? canMoveDownRight = false : canMoveDownLeft = false;
    }
    if (canMoveDownLeft)
    {
        auto below = m_PixelCanvas->GetPixel(x - 1, y - 1);
        m_PixelCanvas->SetPixel(x, y, below);
        m_PixelCanvas->SetPixel(x - 1, y - 1, s_SandColor);
        m_Dirty = true;
    }
    else if (canMoveDownRight)
    {
        auto below = m_PixelCanvas->GetPixel(x + 1, y - 1);
        m_PixelCanvas->SetPixel(x, y, below);
        m_PixelCanvas->SetPixel(x + 1, y - 1, s_SandColor);
        m_Dirty = true;
    }
}

void SandSimulation::UpdateWater(int x, int y)
{
    if (CanMove(*m_PixelCanvas, x, y - 1))
    {
        auto other = m_PixelCanvas->GetPixel(x, y - 1);
        m_PixelCanvas->SetPixel(x, y, other);
        m_PixelCanvas->SetPixel(x, y - 1, s_WaterColor);
        m_Dirty = true;
        return;
    }

    bool canMoveDownLeft = CanMove(*m_PixelCanvas, x - 1, y - 1);
    bool canMoveDownRight = CanMove(*m_PixelCanvas, x + 1, y - 1);
    if (canMoveDownLeft && canMoveDownRight)
    {
        auto fc = App::Instance()->GetFPSCounter().GetFrameCount() % 2;
        fc == 0 ? canMoveDownRight = false : canMoveDownLeft = false;
    }
    if (canMoveDownLeft)
    {
        auto below = m_PixelCanvas->GetPixel(x - 1, y - 1);
        m_PixelCanvas->SetPixel(x, y, below);
        m_PixelCanvas->SetPixel(x - 1, y - 1, s_WaterColor);
        m_Dirty = true;
        return;
    }
    if (canMoveDownRight)
    {
        auto below = m_PixelCanvas->GetPixel(x + 1, y - 1);
        m_PixelCanvas->SetPixel(x, y, below);
        m_PixelCanvas->SetPixel(x + 1, y - 1, s_WaterColor);
        m_Dirty = true;
        return;
    }

    bool canMoveLeft = CanMove(*m_PixelCanvas, x - 1, y);
    bool canMoveRight = CanMove(*m_PixelCanvas, x + 1, y);
    if (canMoveLeft && canMoveRight)
    {
        auto fc = App::Instance()->GetFPSCounter().GetFrameCount() % 2;
        fc == 0 ? canMoveRight = false : canMoveLeft = false;
    }
    if (canMoveLeft)
    {
        auto below = m_PixelCanvas->GetPixel(x - 1, y);
        m_PixelCanvas->SetPixel(x, y, below);
        m_PixelCanvas->SetPixel(x - 1, y, s_WaterColor);
        m_Dirty = true;
        return;
    }
    if (canMoveRight)
    {
        auto other = m_PixelCanvas->GetPixel(x + 1, y);
        m_PixelCanvas->SetPixel(x, y, other);
        m_PixelCanvas->SetPixel(x + 1, y, s_WaterColor);
        m_Dirty = true;
        return;
    }
}
