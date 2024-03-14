#include "MainLayer.h"
#include "Cube.h"
#include "Particles.h"
#include "WorldMap.h"
#include "Asteroids.h"
#include "SandSimulation.h"
#include "Shapes.h"

#include "emscripten.h"

MainLayer::MainLayer() : Layer("Main")
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Camera = std::make_shared<PerspectiveCamera>(
            (float)webGpuWindow->GetWidth() / (float)webGpuWindow->GetHeight());
    m_Camera->SetPosition(glm::vec3(0.0, 0.0, 3.0));

    m_CameraController = new CameraController(m_Camera);
    m_CameraController->SetMovementSpeed(1.0f);

    m_Layers.push_back(new Cube(m_Camera, true));
    m_Layers.push_back(new Particles(m_Camera, false));
    // m_Layers.push_back(new WorldMap(m_Camera, false));
    // m_Layers.push_back(new Asteroids(*m_Camera, false));
    // m_Layers.push_back(new SandSimulation(*webGpuWindow->GetContext(), *m_Camera));
    m_Layers.push_back(new Shapes(*webGpuWindow->GetContext(), *m_Camera));

    for (auto layer : m_Layers)
    {
        App::Instance()->AddLayer(layer);
    }
}

void MainLayer::OnAttach()
{

}

void MainLayer::OnDetach()
{

}

void MainLayer::OnUpdate(float deltaTime)
{
    m_BlinkTimer += deltaTime;
    if (m_BlinkTimer >= 1.0f)
    {
        m_SwapColor = !m_SwapColor;
        if (m_SwapColor)
        {
            m_BlinkTextColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
        }
        else
        {
            m_BlinkTextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0);
        }
        m_BlinkTimer = 0.0f;
    }

    m_CameraController->Update(deltaTime);
}

static void HyperLink(const char *label, const char *url)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4, 0.4, 1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    if (ImGui::Button(label))
    {
        std::string openUrl = "window.open('" + std::string(url) + "', '_blank')";
        emscripten_run_script(openUrl.c_str());
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

template<typename T, typename F>
static void LayerDetails(Layer *layer, F drawDetails)
{
    auto layerType = dynamic_cast<T *>(layer);
    if (layerType == nullptr)
    {
        return;
    }
    ImGui::Separator();
    drawDetails(layerType);
}

void MainLayer::OnImGuiRender()
{
    ImVec2 appWindowSize = ImVec2((float)App::Instance()->GetWindow()->GetWidth(),
            (float)App::Instance()->GetWindow()->GetHeight());
    const int padding = 10;

    auto width = appWindowSize.x / 5;
    auto height = appWindowSize.y / 3;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);

    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(padding, padding), ImGuiCond_FirstUseEver);
    ImGui::Begin("Willehard Korander | Software Engineer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("CONTACT");
    ImGui::Text("Email:              ");
    ImGui::SameLine(0.0, 0.0);
    HyperLink("willehard@gmail.com", "mailto:willehard@gmail.com");
    ImGui::Text("GitHub:             ");
    ImGui::SameLine(0.0, 0.0);
    HyperLink("https://github.com/wehard", "https://github.com/wehard");
    ImGui::Text("LinkedIn:           ");
    ImGui::SameLine(0.0, 0.0);
    HyperLink("https://www.linkedin.com/in/willehard", "https://www.linkedin.com/in/willehard");
    ImGui::Spacing();
    ImGui::Text("EXPERIENCE");
    ImGui::Text("[OCT 23 - PRESENT]: Senior Software Engineer, Unity Technologies, Helsinki");
    ImGui::Text("[APR 21 - OCT 23]:  Software Engineer, Unity Technologies, Helsinki");
    ImGui::Text("[AUG 20 - APR 21]:  Software Developer Intern, Unity Technologies, Helsinki");
    ImGui::Spacing();
    ImGui::Text("EDUCATION");
    // set color of text
    ImGui::Text("[OCT 19 - JUL 20]:  ");
    ImGui::SameLine(0.0, 0.0);
    ImGui::TextColored(m_BlinkTextColor, "Hive Helsinki");
    ImGui::SameLine(0.0, 0.0);
    ImGui::Text(", Computer Science");
    ImGui::Text("[2018]:             Introduction to databases, University of Helsinki, Open University");
    ImGui::Text("[2016]:             Advanced course in programming, University of Helsinki, Open University");
    ImGui::Text("[2016]:             Introduction to programming, University of Helsinki, Open University");
    ImGui::Text("[AUG 03 - JUL 07]:  Theatre Academy, Helsinki, Master of Arts in Acting");
    ImGui::Spacing();
    ImGui::Text("SKILLS");
    ImGui::Text(
            "C/C++, C#, JS/TS\nOpenGL, WebGPU, Vulkan, Unity\nGit, Linux, macOS, Windows\nWeb, React, Node.js, GraphQL, REST\nGCP, Jira, Confluence");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(appWindowSize.x - width - padding, padding + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(width, appWindowSize.y / 2), ImGuiCond_FirstUseEver);
    ImGui::Begin("Layers");

    for (auto layer : m_Layers)
    {
        auto enabled = layer->IsEnabled();
        bool selected = m_SelectedLayer == layer;
        // if (ImGui::Checkbox(("##" + layer->GetName()).c_str(), &enabled))
        if (ImGui::RadioButton(("##" + layer->GetName()).c_str(), enabled))
        {
            layer->SetEnabled(!layer->IsEnabled());
            for (auto other : m_Layers)
            {
                if (other == layer)
                    continue;
                other->SetEnabled(false);
            }
        }
        ImGui::SameLine();
        if (ImGui::Selectable(layer->GetName().c_str(), &selected))
        {
            m_SelectedLayer = layer;
        }

    }

    if (m_SelectedLayer)
    {
        LayerDetails<Cube>(m_SelectedLayer, [](Cube *cube)
        {
            ImGui::Text("It's a cube.");
        });

        LayerDetails<Particles>(m_SelectedLayer, [](Particles *cube)
        {
            ImGui::Text("...");
        });

        LayerDetails<WorldMap>(m_SelectedLayer, [](WorldMap *worldMap)
        {
            ImGui::Text("Particle count: %d", worldMap->GetParticleCount());
            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                worldMap->ResetParticles();
            }
            ImGui::SliderFloat("Rotation speed", &worldMap->RotationSpeed, -100.0, 100.0);
            auto texture = worldMap->GetTexture();
            auto bump = worldMap->GetHeightMap();
            auto aspect = (float)texture->GetHeight() / (float)texture->GetWidth();
            auto bumpAspect = (float)bump->GetHeight() / (float)bump->GetWidth();
            auto contentWidth = ImGui::GetWindowContentRegionWidth();
            ImGui::Image((ImTextureID)texture->GetTextureView(), ImVec2(contentWidth, contentWidth * aspect));
            ImGui::Image((ImTextureID)bump->GetTextureView(), ImVec2(contentWidth, contentWidth * bumpAspect));
        });
    }
    ImGui::End();
    DrawDebugPanel(*m_Camera, true);
    ImGui::PopStyleVar();
}

void MainLayer::OnEvent(Figment::AppEvent event, void *eventData)
{
    auto ev = (Figment::WindowResizeEventData *)eventData;
    m_Camera->Resize((float)ev->Width, (float)ev->Height);
}
