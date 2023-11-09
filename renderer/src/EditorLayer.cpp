#include "EditorLayer.h"
#include "Random.h"

#include "imgui.h"
#include "imgui_impl_wgpu.h"
#include "imgui_impl_glfw.h"
#include "glm.hpp"
#include "Input.h"
#include "App.h"

namespace Figment
{
    Figment::EditorLayer::EditorLayer()
            : Layer("EditorLayer")
    {
        auto m_Window = App::Instance()->GetWindow();
        m_Scene = new Scene(m_Window->GetFramebufferWidth(), m_Window->GetFramebufferHeight());

        for (int i = 0; i < MaxQuadCount; i++)
        {
            auto quadEntity = m_Scene->CreateEntity("Quad");
            auto &transformQuad = quadEntity.GetComponent<TransformComponent>();
            transformQuad.Position = glm::vec3(Random::Float(-100.0, 100.0), Random::Float(-100.0, 100.0),
                    Random::Float(-100.0, 100.0));
            auto &quad = quadEntity.AddComponent<QuadComponent>();
            quad.Color = Random::Color();
        }

        for (int j = 0; j < MaxCircleCount; j++)
        {
            auto circleEntity = m_Scene->CreateEntity("Circle");
            auto &transformCircle = circleEntity.GetComponent<TransformComponent>();
            transformCircle.Position = glm::vec3(Random::Float(-100.0, 100.0), Random::Float(-100.0, 100.0),
                    Random::Float(-100.0, 100.0));
            auto &circle = circleEntity.AddComponent<CircleComponent>(2.0f);
            circle.Color = Random::Color();
        }
    }

    Figment::EditorLayer::~EditorLayer()
    {
        delete m_Scene;
    }

    void Figment::EditorLayer::OnAttach()
    {

    }

    void Figment::EditorLayer::OnDetach()
    {

    }

    void Figment::EditorLayer::OnUpdate(float deltaTime)
    {
        auto m_Window = App::Instance()->GetWindow();
        m_Scene->OnUpdate(deltaTime, Input::GetMousePosition(), glm::vec2(m_Window->GetWidth(), m_Window->GetHeight()));

        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse || io.WantCaptureKeyboard)
        {
            return;
        }

        if (Input::GetKeyDown(GLFW_KEY_I))
        {
            Entity e = m_Scene->CreateEntity("New");
            auto &t = e.GetComponent<TransformComponent>();
            glm::vec3 p = m_Scene->GetCameraController()->GetCamera()->ScreenToWorldSpace(Input::GetMousePosition(),
                    glm::vec2(m_Window->GetWidth(), m_Window->GetHeight()));
            t.Position = p;
            auto &b = e.AddComponent<VerletBodyComponent>();
            b.m_PreviousPosition = t.Position;
            auto md = Input::GetMouseDelta() * 0.01f;
            b.m_PreviousPosition.x -= md.x;
            b.m_PreviousPosition.y += md.y;
        }
        if (m_SelectedEntity && (Input::GetKeyDown(GLFW_KEY_DELETE) || Input::GetKeyDown(GLFW_KEY_BACKSPACE)))
        {
            SelectEntity({});
            m_Scene->DestroyEntity(m_SelectedEntity);
        }

        if (Input::GetButtonDown(GLFW_MOUSE_BUTTON_LEFT))
        {
            SelectEntity(m_Scene->GetHoveredEntity());
        }

    }

    static void DrawVec3(const char *name, glm::vec3 *value, bool *syncValues)
    {
        glm::vec3 tempValue = *value;
        ImGui::DragFloat3(name, &tempValue[0], 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::SameLine();
        ImGui::Checkbox("Sync", syncValues);

        if (*syncValues)
        {
            for (size_t i = 0; i < 3; i++)
            {
                if (tempValue[i] != value[0][i])
                {
                    *value = glm::vec3(tempValue[i]);
                    break;
                }
            }
        }
        else
        {
            *value = tempValue;
        }
    }

    static void DrawWGSLShaderEditor(WebGPUShader &shader)
    {
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Once);
        ImGui::Begin("WebGPU Shader Editor");
        if (ImGui::BeginTabBar("TabBar"))
        {
            ImVec2 contentSize = ImGui::GetContentRegionAvail();
            if (ImGui::BeginTabItem("WGSL"))
            {
                ImGui::InputTextMultiline("##ShaderCode", shader.GetShaderSource().data(),
                        shader.GetShaderSource().size(),
                        contentSize, ImGuiInputTextFlags_AllowTabInput, nullptr, nullptr);
                ImGui::EndTabItem();
            }

            // Create the second tab
            if (ImGui::BeginTabItem("Compute"))
            {
                std::string computeShaderSource = "Hello, World!";
                ImGui::InputTextMultiline("##ComputeCode", computeShaderSource.data(), computeShaderSource.size(),
                        contentSize);
                ImGui::EndTabItem();
            }

            // End the tab bar
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    static void DrawScenePanel(const std::vector<Entity> &entities,
            const std::function<void(Entity)> &selectEntity = nullptr)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
        ImGui::Begin("Scene");
        for (auto entity : entities)
        {
            ImGui::PushID((int)entity.GetHandle());
            auto &info = entity.GetComponent<InfoComponent>();
            if (ImGui::Selectable(info.m_Name.c_str()))
            {
                selectEntity(entity);
            }
            ImGui::PopID();
        }
        ImGui::End();
    }

    static void DrawInfoComponent(InfoComponent &info)
    {
        char buf[128];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%s", info.m_Name.c_str());
        if (ImGui::InputText("##Name", buf, sizeof(buf)))
        {
            info.m_Name = std::string(buf);
        }
    }

    static void DrawTransformComponent(TransformComponent &transform)
    {
        ImGui::DragFloat3("Position", (float *)&transform.Position.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat3("Rotation", (float *)&transform.Rotation.x, 0.1f, 0.0f, 0.0f, "%.2f");

        static bool syncScale = true;
        DrawVec3("Scale", &transform.Scale, &syncScale);
    }

    static void DrawQuadComponent(QuadComponent &quad)
    {
        ImGui::ColorEdit4("Color", (float *)&quad.Color);
    }

    static void DrawCircleComponent(CircleComponent &circle)
    {
        ImGui::ColorEdit4("Color", (float *)&circle.Color);
        ImGui::DragFloat("Radius", &circle.Radius, 0.1f, 0.1f, FLT_MAX, "%.2f");
    }

    static void DrawEntityInspectorPanel(Entity entity)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 300), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
        ImGui::Begin("Entity Inspector");

        if (!entity)
        {
            ImGui::End();
            return;
        }

        if (entity.HasComponent<InfoComponent>())
            DrawInfoComponent(entity.GetComponent<InfoComponent>());

        if (entity.HasComponent<TransformComponent>())
            DrawTransformComponent(entity.GetComponent<TransformComponent>());

        if (entity.HasComponent<QuadComponent>())
            DrawQuadComponent(entity.GetComponent<QuadComponent>());

        if (entity.HasComponent<CircleComponent>())
            DrawCircleComponent(entity.GetComponent<CircleComponent>());

        ImGui::End();
    }

    static void DrawEditorCameraSection(CameraController &cameraController)
    {
        auto cameraSettings = cameraController.GetCamera()->GetSettings();
        ImGui::Text("Speed: %.2f", cameraController.GetSpeed());
        ImGui::Text("Rotation speed: %.2f", cameraController.GetRotationSpeed());
        ImGui::Separator();
        ImGui::Text("Aspect: %.2f", cameraSettings.AspectRatio);
        ImGui::Text("Near: %.2f", cameraSettings.NearClip);
        ImGui::Text("Far: %.2f", cameraSettings.FarClip);
        ImGui::Text("Yaw: %.2f", cameraSettings.Yaw);
        ImGui::Text("Pitch: %.2f", cameraSettings.Pitch);
        ImGui::Text("Position: %.2f %.2f %.2f", cameraSettings.Position.x,
                cameraSettings.Position.y, cameraSettings.Position.z);
        ImGui::Text("Forward: %.2f %.2f %.2f", cameraSettings.Forward.x,
                cameraSettings.Forward.y, cameraSettings.Forward.z);
        ImGui::Text("Right: %.2f %.2f %.2f", cameraSettings.Right.x,
                cameraSettings.Right.y, cameraSettings.Right.z);
        ImGui::Text("Up: %.2f %.2f %.2f", cameraSettings.Up.x,
                cameraSettings.Up.y, cameraSettings.Up.z);
    }

    static void DrawInfoPanel(Scene &scene, Entity selectedEntity)
    {
        auto window = App::Instance()->GetWindow();

        auto cameraController = scene.GetCameraController();
        glm::vec2 mousePosition = Input::GetMousePosition();
        glm::vec2 ndc = glm::vec2((mousePosition.x / ((float)window->GetWidth() * 0.5)) - 1.0,
                (mousePosition.y / ((float)window->GetHeight() * 0.5)) - 1.0);
        glm::vec3 mw = cameraController->GetCamera()->ScreenToWorldSpace(mousePosition,
                glm::vec2(window->GetWidth(), window->GetHeight()));

        ImGui::SetNextWindowPos(ImVec2(window->GetWidth() - 400, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Always);
        ImGui::Begin("Info");
        if (ImGui::TreeNode("Window"))
        {
            if (ImGui::BeginTable("My Table", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Viewport");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%dx%d", window->GetWidth(), window->GetHeight());
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Framebuffer");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%dx%d", window->GetFramebufferWidth(), window->GetFramebufferHeight());
                ImGui::EndTable();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Camera"))
        {
            DrawEditorCameraSection(*scene.GetCameraController());
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Input"))
        {
            if (ImGui::BeginTable("Mouse", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Position");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f %.2f", mousePosition.x, mousePosition.y);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Delta");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f %.2f", Input::GetMouseDelta().x, Input::GetMouseDelta().y);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("NDC");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f %.2f", ndc.x, ndc.y);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("World");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f %.2f %.2f", mw.x, mw.y, mw.z);
                ImGui::EndTable();

                ImGui::Separator();

                ImGui::BeginTable("Entity Selection", 2);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Entity");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", scene.m_HoveredId);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Selected");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", selectedEntity ? selectedEntity.GetHandle() : -1);
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Renderer"))
        {
            auto stats = WebGPURenderer::GetStats();
            ImGui::Text("Draw calls: %d", stats.DrawCalls);
            ImGui::Text("Vertex count: %d", stats.VertexCount);
            ImGui::Text("Quad count: %d", stats.QuadCount);
            ImGui::Text("Circle count: %d", stats.CircleCount);
            ImGui::Separator();
            auto fpsCounter = App::Instance()->GetFPSCounter();
            ImGui::Text("FPS: %.2f", fpsCounter.GetFPS());
            ImGui::Text("Frame time: %.2f ms", fpsCounter.GetFrameTime());
            ImGui::Text("Frame count: %d", fpsCounter.GetFrameCount());
            ImGui::TreePop();
        }
        ImGui::End();
    }

    void Figment::EditorLayer::OnImGuiRender()
    {
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawInfoPanel(*m_Scene, m_SelectedEntity);

        DrawScenePanel(m_Scene->GetEntities(), [this](Entity entity)
        {
            SelectEntity(entity);
        });
        DrawEntityInspectorPanel(m_SelectedEntity);
        // DrawWGSLShaderEditor(*m_Renderer->GetShader());
    }

    void EditorLayer::SelectEntity(Entity entity)
    {
        m_SelectedEntity = entity;
    }

    void EditorLayer::OnEvent(AppEvent event, void *eventData)
    {
        switch (event)
        {
        case AppEvent::None:
            break;
        case AppEvent::WindowClose:
            break;
        case AppEvent::WindowResize:
        {
            auto ev = (WindowResizeEventData *)eventData;
            m_Scene->OnResize(ev->width, ev->height);
            break;
        }
        case AppEvent::WindowFocus:
            break;
        case AppEvent::WindowLostFocus:
            break;
        case AppEvent::WindowMoved:
            break;
        }
    }
}


