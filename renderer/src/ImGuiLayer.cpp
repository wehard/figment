#include "ImGuiLayer.h"

#include "imgui.h"
#include "imgui_impl_wgpu.h"
#include "imgui_impl_glfw.h"
#include "glm.hpp"
#include "Input.h"
#include "App.h"

namespace Figment
{
    Figment::ImGuiLayer::ImGuiLayer()
            : Layer("ImGuiLayer")
    {
        auto m_Window = App::Instance()->GetWindow();
        m_Scene = new Scene(m_Window->GetFramebufferWidth(), m_Window->GetFramebufferHeight());

        auto circleEntity = m_Scene->CreateEntity("Circle");
        auto &transformCircle = circleEntity.GetComponent<TransformComponent>();
        transformCircle.Position = glm::vec3(0.0f, 0.0f, 0.0f);
        transformCircle.Scale = glm::vec3(20.0f);
        circleEntity.AddComponent<CircleComponent>(20.0f);

        auto quadEntity = m_Scene->CreateEntity("Quad");
        auto &transformQuad = quadEntity.GetComponent<TransformComponent>();
        transformQuad.Position = glm::vec3(0.0f, 0.0f, 0.0f);
        transformQuad.Scale = glm::vec3(20.0f);
        quadEntity.AddComponent<QuadComponent>();
    }

    Figment::ImGuiLayer::~ImGuiLayer()
    {
        delete m_Scene;
    }

    void Figment::ImGuiLayer::OnAttach()
    {

    }

    void Figment::ImGuiLayer::OnDetach()
    {

    }

    void Figment::ImGuiLayer::OnUpdate(float deltaTime)
    {
        auto m_Window = App::Instance()->GetWindow();
        m_Scene->Update(deltaTime, Input::GetMousePosition(), glm::vec2(m_Window->GetWidth(), m_Window->GetHeight()));

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
                float f = value[0][i];
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

    static void DrawEntitiesPanel(const std::vector<Entity> &entities,
            const std::function<void(Entity)> &selectEntity = nullptr)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
        ImGui::Begin("Entities");
        int i = 0;
        for (auto entity : entities)
        {
            ImGui::PushID(i);
            auto &info = entity.GetComponent<InfoComponent>();
            char buf[128];
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "%-20s", info.m_Name.c_str());
            if (ImGui::Selectable(buf))
            {
                selectEntity(entity);
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::SetTooltip("Entity UUID: %llu\nEntt handle: %d",
                        (uint64_t)entity.GetComponent<IdComponent>().UUID,
                        entity.GetHandle());
            }
            ImGui::PopID();
            i++;
        }
        ImGui::End();
    }

    static void DrawInspectorPanel(Entity entity)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 300), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
        ImGui::Begin("Inspector");

        if (!entity)
        {
            ImGui::Text("No entity selected");
            ImGui::End();
            return;
        }

        auto &info = entity.GetComponent<InfoComponent>();

        char buf[128];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%s", info.m_Name.c_str());
        if (ImGui::InputText("##Name", buf, sizeof(buf)))
        {
            info.m_Name = std::string(buf);
        }

        auto &transform = entity.GetComponent<TransformComponent>();
        ImGui::DragFloat3("Position", (float *)&transform.Position.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat3("Rotation", (float *)&transform.Rotation.x, 0.1f, 0.0f, 0.0f, "%.2f");

        static bool syncScale = true;
        DrawVec3("Scale", &transform.Scale, &syncScale);

        ImGui::Separator();
        ImGui::ColorEdit4("Color", (float *)&entity.GetComponent<ColorComponent>().m_Color.x);

        if (entity.HasComponent<VerletBodyComponent>())
        {
            ImGui::Separator();
            auto &body = entity.GetComponent<VerletBodyComponent>();
            ImGui::Text("Verlet Body");
            ImGui::DragFloat3("Previous position", (float *)&body.m_PreviousPosition.x);
            ImGui::Text("Velocity: x %f y %f z %f", body.m_Velocity.x, body.m_Velocity.y, body.m_Velocity.z);
        }

        if (entity.HasComponent<CircleComponent>())
        {
            ImGui::Separator();
            auto &circle = entity.GetComponent<CircleComponent>();
            ImGui::Text("Circle");
            ImGui::DragFloat("Radius", &circle.Radius, 0.1f, 0.1f, FLT_MAX, "%.2f");
        }
        ImGui::End();
    }

    void Figment::ImGuiLayer::OnImGuiRender()
    {
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto m_Window = App::Instance()->GetWindow();

        auto camera = m_Scene->GetCameraController()->GetCamera();
        size_t cameraWindowWidth = 400;
        ImGui::SetNextWindowPos(ImVec2(m_Window->GetWidth() / 2 - cameraWindowWidth / 2, 0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(cameraWindowWidth, 0), ImGuiCond_Once);
        ImGui::Begin("Camera");
        glm::vec3 cameraPosition = camera->GetPosition();
        ImGui::Text("%-12s %f %f %f", "Position", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        ImGui::Text("%-12s %f %f %f", "Forward", camera->m_Forward.x, camera->m_Forward.y, camera->m_Forward.z);
        ImGui::Text("%-12s %f", "Yaw", camera->m_Yaw);
        ImGui::Text("%-12s %f", "Pitch", camera->m_Pitch);
        ImGui::Text("%-12s %f", "Aspect", camera->GetAspectRatio());

        ImGui::Separator();

        if (ImGui::DragFloat3("Position", (float *)&cameraPosition.x, 0.1f, 0.0f, 0.0f, "%.2f"))
        {
            camera->m_Position = cameraPosition;
        }
        ImGui::End();

        glm::vec2 mousePosition = Input::GetMousePosition();
        glm::vec2 ndc = glm::vec2((mousePosition.x / ((float)m_Window->GetWidth() * 0.5)) - 1.0,
                (mousePosition.y / ((float)m_Window->GetHeight() * 0.5)) - 1.0);
        glm::vec3 mw = camera->ScreenToWorldSpace(mousePosition,
                glm::vec2(m_Window->GetWidth(), m_Window->GetHeight()));

        ImGui::SetNextWindowPos(ImVec2(m_Window->GetWidth() - 400, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Always);
        ImGui::Begin("Info");
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Window"))
        {
            if (ImGui::BeginTable("My Table", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Viewport");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%dx%d", m_Window->GetWidth(), m_Window->GetHeight());
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Framebuffer");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%dx%d", m_Window->GetFramebufferWidth(), m_Window->GetFramebufferHeight());
                ImGui::EndTable();
            }

            ImGui::TreePop();
        }

        auto fps = App::Instance()->GetFps();
        ImGui::Text("FPS: %.2f", fps);

        if (ImGui::BeginListBox("Mouse"))
        {
            ImGui::Text("Position: %.2f %.2f", mousePosition.x, mousePosition.y);
            ImGui::Text("Delta: %.2f %.2f", Input::GetMouseDelta().x, Input::GetMouseDelta().y);
            ImGui::Text("NDC: %.2f %.2f", ndc.x, ndc.y);
            ImGui::Text("World: %.2f %.2f %.2f", mw.x, mw.y, mw.z);
            ImGui::EndListBox();
        }
        ImGui::Text("Entity: %d", m_Scene->m_HoveredId);
        ImGui::Text("Selected: %d", m_SelectedEntity ? m_SelectedEntity.GetHandle() : -1);
        ImGui::End();

        DrawEntitiesPanel(m_Scene->GetEntities(), [this](Entity entity)
        {
            SelectEntity(entity);
        });
        DrawInspectorPanel(m_SelectedEntity);
        // DrawWGSLShaderEditor(*m_Renderer->GetShader());

        static bool showStyleEditor = false;
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("Figment"))
        {
            if (ImGui::MenuItem("New Entity", "Ctrl+N"))
            {
                auto entity = m_Scene->CreateEntity("New Entity");
                entity.AddComponent<ColorComponent>();
                entity.AddComponent<QuadComponent>();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            auto text = showStyleEditor ? "Hide Style Editor" : "Show Style Editor";
            if (ImGui::MenuItem(text, "Ctrl+S"))
            {
                showStyleEditor = !showStyleEditor;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        if (showStyleEditor)
        {
            ImGui::SetNextWindowPos(ImVec2(400, 200), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Once);
            ImGui::Begin("Style Editor");
            auto style = ImGui::GetStyle();
            ImGui::ShowStyleEditor(&style);
            ImGui::End();
        }
    }

    void ImGuiLayer::SelectEntity(Entity entity)
    {
        m_SelectedEntity = entity;
    }

    void ImGuiLayer::OnEvent(AppEvent event, void *eventData)
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


