#include "MainLayer.h"
#include "Cube.h"
#include "Galaxy.h"
#include "Worlds.h"
#include "Asteroids.h"
#include "GameOfLife.h"
#include "Shapes.h"
#include "MetaPlayer.h"

#include <emscripten.h>

MainLayer::MainLayer(const char *initialLayerName) : Layer("Main")
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Camera = std::make_shared<PerspectiveCamera>(
            (float)webGpuWindow->GetWidth() / (float)webGpuWindow->GetHeight());
    m_Camera->SetPosition(glm::vec3(0.0, 0.0, 2.0));

    m_CameraController = std::make_shared<CameraController>(m_Camera);
    m_CameraController->SetMovementSpeed(1.0f);

    m_Layers.push_back(new Worlds(*m_Camera, false));
    m_Layers.push_back(new Galaxy(*m_Camera, false));
    m_Layers.push_back(new GameOfLife(*webGpuWindow->GetContext<WebGPUContext>(), *m_Camera));
    m_Layers.push_back(new MetaPlayer(false));
    // m_Layers.push_back(new Asteroids(*m_Camera, false));
    m_Layers.push_back(new Shapes(*webGpuWindow->GetContext<WebGPUContext>()));
    // m_Layers.push_back(new Cube(m_Camera, true));

    for (auto layer : m_Layers)
    {
        App::Instance()->AddLayer(layer);
        if (strcmp(layer->GetName().c_str(), initialLayerName) == 0)
        {
            m_SelectedLayer = layer;
            layer->SetEnabled(true);
        }
        else
        {
            layer->SetEnabled(false);
        }
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

    // m_CameraController->Update(deltaTime);
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

static void DrawTransform(glm::vec3 &position, glm::vec3 &rotation, glm::vec3 &scale)
{
    ImGui::DragFloat3("Position", &position[0], 0.01f);
    ImGui::DragFloat3("Rotation", &rotation[0], 0.1f);
    ImGui::DragFloat3("Scale", &scale[0], 0.01f);
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

    ImGui::Begin("Willehard Korander | Software Engineer", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus
                    | ImGuiWindowFlags_NoFocusOnAppearing);
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
    ImGui::Text("[AUG 24 - PRESENT]: Senior Software Developer, Supponor | TGI Sport, Espoo");
    ImGui::Text("[OCT 23 - APR 24]:  Senior Software Engineer, Unity Technologies, Helsinki");
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
    ImGui::Dummy({ 0, 10 });
    ImGui::Separator();
    ImGui::Dummy({ 0, 10 });
    ImGui::TextWrapped(
            "This site is powered by the Figment engine. Figment is an in-development, C++17, real-time 3D engine supporting WebGPU.");
    ImGui::Spacing();
    HyperLink("Source code on GitHub", "https://github.com/wehard/figment");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(appWindowSize.x - width - padding, padding + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(width, appWindowSize.y / 1.5), ImGuiCond_FirstUseEver);
    ImGui::Begin("Layers", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);

    for (auto layer : m_Layers)
    {
        auto enabled = layer->IsEnabled();
        bool selected = m_SelectedLayer == layer;
        if (ImGui::RadioButton(("##" + layer->GetName()).c_str(), enabled))
        {
            m_SelectedLayer = layer;
            layer->SetEnabled(!layer->IsEnabled());
            for (auto other : m_Layers)
            {
                if (other == layer)
                    continue;
                other->SetEnabled(false);
            }

            Figment::Window::ResizeEventData ev = { (int)appWindowSize.x, (int)appWindowSize.y };
            layer->OnEvent(Figment::AppEvent::WindowResize, &ev);
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

        LayerDetails<Galaxy>(m_SelectedLayer, [](Galaxy *galaxy)
        {
            ImGui::SliderFloat("Particle size", &galaxy->m_ParticleSize, 0.001, 0.1);
            DrawTransform(galaxy->m_Position, galaxy->m_Rotation, galaxy->m_Scale);
        });

        LayerDetails<Worlds>(m_SelectedLayer, [](Worlds *worldMap)
        {
            DrawTransform(worldMap->Position, worldMap->Rotation, worldMap->Scale);
            ImGui::Spacing();
            ImGui::Checkbox("Cycle", &worldMap->Cycle);
            ImGui::SameLine();
            ImGui::Checkbox("Rotate", &worldMap->Rotate);
            ImGui::DragFloat("Rotation speed", &worldMap->RotationSpeed, 0.01, -100.0, 100.0);
            ImGui::DragFloat("Particle size", &worldMap->ParticleSize, 0.001, 0.001, 0.1);
            ImGui::DragFloat("Bump multiplier", &worldMap->BumpMultiplier, 0.01, -1.0, 1.0);
            ImGui::Spacing();
            ImGui::Text("Particle count: %d", worldMap->GetParticleCount());
        });

        LayerDetails<GameOfLife>(m_SelectedLayer, [](GameOfLife *gameOfLife)
        {
            if (ImGui::Button("Randomize"))
            {
                gameOfLife->Randomize();
            }
        });

        LayerDetails<MetaPlayer>(m_SelectedLayer, [](MetaPlayer *metaGameSim)
        {
            ImGui::Text("MetaPlayer");
            ImGui::Spacing();
            ImGui::TextWrapped(
                    "A* search attempts to find optimal sequence of actions to maximize the selected game variable.");
            ImGui::Spacing();
            ImGui::TextWrapped(
                    "Select a game variable to maximize and the amount to maximize it to and press Start to perform search.");
            ImGui::Spacing();
            ImGui::TextWrapped(
                    "Meta game can be manually played by pressing the top buttons:");
            ImGui::BulletText("Play Game: Play core game");
            ImGui::BulletText("Buy Parts: Buy parts used for upgrading items");
            ImGui::BulletText("Upgrade Weapon: Upgrade weapon level");
            ImGui::BulletText("Upgrade Vehicle: Upgrade vehicle level");
            ImGui::BulletText("Reset: Reset game state");
            ImGui::Spacing();

            ImGui::Spacing();
            HyperLink("Source code on GitHub",
                    "https://github.com/wehard/figment/tree/main/portfolio/src/MetaPlayer");
            HyperLink("A* implementation on GitHub",
                    "https://github.com/wehard/figment/blob/main/engine/src/utils/AStar.h");
        });

        LayerDetails<Shapes>(m_SelectedLayer, [](Shapes *shapes)
        {
            ImGui::Checkbox("Auto A*", &shapes->RunningAStar);
            if (!shapes->RunningAStar && ImGui::Button("Run A*"))
            {
                shapes->RunAStar();
            }
        });
    }
    ImGui::End();
    DrawDebugPanel(*m_Camera, true);
    ImGui::PopStyleVar();
}

void MainLayer::OnEvent(Figment::AppEvent event, void *eventData)
{
    auto ev = (Figment::Window::ResizeEventData *)eventData;
    m_Camera->Resize((float)ev->Width, (float)ev->Height);
}
