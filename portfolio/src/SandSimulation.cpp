#include "SandSimulation.h"
#include "ComputePass.h"

SandSimulation::SandSimulation(Figment::WebGPUContext &context, PerspectiveCamera &camera) : Layer("SandSimulation"),
        m_Context(context), m_Camera(camera)
{
    PixelCanvasDescriptor descriptor = { s_Width, s_Height, false };
    m_PixelCanvas = new PixelCanvas(context, &descriptor);

    m_PixelCanvas->Fill(s_AirColor);

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
    auto r = Random::Float();
    m_PixelCanvas->SetPixel(
            ((s_Width / 2) + (uint32_t)(10 * r)) + glm::sin(App::Instance()->GetTimeSinceStart()) * 30,
            180, App::Instance()->GetTimeSinceStart() > 5.0 ? s_WaterColor : s_SandColor);

    for (int y = 1; y < s_Height; y++)
    {
        for (int x = 0; x < s_Width; x++)
        {
            auto pixel = m_PixelCanvas->GetPixel(x, y);
            if (pixel == s_SandColor)
            {
                UpdateSandParticle(x, y);
            }
            else if (pixel == s_WaterColor)
            {
                UpdateWaterParticle(x, y);
            }
        }
    }
    if (m_Dirty)
        m_PixelCanvas->UpdateTexture();

    // auto m_ComputeBindGroup = new BindGroup(m_Context.GetDevice(), WGPUShaderStage_Compute);
    // m_ComputeBindGroup->Bind(*m_UniformBuffer);
    // m_ComputeBindGroup->BindStorage(m_PixelCanvas->GetTexture(), WGPUStorageTextureAccess_ReadOnly);
    // m_ComputeBindGroup->BindStorage(m_PixelCanvas->GetComputeTexture(), WGPUStorageTextureAccess_WriteOnly);
    //
    // auto m_SimulatePipeline = new ComputePipeline(m_Context.GetDevice(), *m_ComputeBindGroup);
    // m_SimulatePipeline->Build("simulate", m_ComputeShader->GetShaderModule());
    //
    // ComputePass computePass(m_Context.GetDevice(), m_SimulatePipeline, m_ComputeBindGroup);
    // computePass.Begin();
    // computePass.Dispatch("simulate", m_PixelCanvas->GetWidth(), m_PixelCanvas->GetHeight());
    // computePass.End();

    m_PixelCanvas->OnUpdate(m_Camera, deltaTime);

}

void SandSimulation::OnImGuiRender()
{

}

void SandSimulation::OnEvent(Figment::AppEvent event, void *eventData)
{

}

void SandSimulation::UpdateSandParticle(int x, int y)
{
    auto bx = x;
    auto by = y - 1;
    if (m_PixelCanvas->GetPixel(bx, by) == s_AirColor)
    {
        m_PixelCanvas->SwapPixels(x, y, bx, by);
        m_Dirty = true;
    }
    else if (m_PixelCanvas->GetPixel(bx + 1, by) == s_AirColor)
    {
        m_PixelCanvas->SwapPixels(x, y, bx + 1, by);
        m_Dirty = true;
    }
    else if (m_PixelCanvas->GetPixel(bx - 1, by) == s_AirColor)
    {
        m_PixelCanvas->SwapPixels(x, y, bx - 1, by);
        m_Dirty = true;
    }
}

void SandSimulation::UpdateWaterParticle(int x, int y)
{
    if (m_PixelCanvas->GetPixel(x, y - 1) == s_AirColor) // down
    {
        m_PixelCanvas->SwapPixels(x, y, x, y - 1);
        m_Dirty = true;
    }
    else if (m_PixelCanvas->GetPixel(x + 1, y - 1) == s_AirColor)
    {
        m_PixelCanvas->SwapPixels(x, y, x + 1, y - 1);
        m_Dirty = true;
    }
    else if (m_PixelCanvas->GetPixel(x - 1, y - 1) == s_AirColor)
    {
        m_PixelCanvas->SwapPixels(x, y, x - 1, y - 1);
        m_Dirty = true;
    }
    else if (m_PixelCanvas->GetPixel(x + 1, y) == s_AirColor || m_PixelCanvas->GetPixel(x + 1, y) == s_WaterColor)
    {
        m_PixelCanvas->SwapPixels(x, y, x + 1, y);
        m_Dirty = true;
    }
    else if (m_PixelCanvas->GetPixel(x - 1, y) == s_AirColor || m_PixelCanvas->GetPixel(x - 1, y) == s_WaterColor)
    {
        m_PixelCanvas->SwapPixels(x, y, x - 1, y);
        m_Dirty = true;
    }
}
