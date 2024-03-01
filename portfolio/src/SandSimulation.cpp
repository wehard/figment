#include "SandSimulation.h"

SandSimulation::SandSimulation(Figment::WebGPUContext &context, PerspectiveCamera &camera) : Layer("SandSimulation"),
        m_Context(context), m_Camera(camera),
        m_Canvas(context, 320, 200)
{
    m_PixelCanvas = new PixelCanvas(context, 320, 200);

    m_PixelCanvas->SetPixel(0, 0, 0xff00ffff);
    m_PixelCanvas->UpdateTexture();
}

void SandSimulation::OnAttach()
{

}

void SandSimulation::OnDetach()
{

}

void SandSimulation::OnUpdate(float deltaTime)
{
    m_PixelCanvas->OnUpdate(m_Camera, deltaTime);
}

void SandSimulation::OnImGuiRender()
{

}

void SandSimulation::OnEvent(Figment::AppEvent event, void *eventData)
{

}
