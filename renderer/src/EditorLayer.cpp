#include "EditorLayer.h"

#include "imgui.h"
#include "imgui_impl_wgpu.h"
#include "imgui_impl_glfw.h"
#include "glm.hpp"
#include "Input.h"
#include "App.h"
#include "WebGPUWindow.h"
#include "Utils.h"

namespace Figment
{
    EditorLayer::EditorLayer()
            : Layer("EditorLayer")
    {
        const auto window = std::dynamic_pointer_cast<WebGPUWindow>(App::Instance()->GetWindow());
        m_Context = window->GetContext();

        auto width = (float)window->GetFramebufferWidth();
        auto height = (float)window->GetFramebufferHeight();
        m_Scene = CreateUniquePtr<Scene>(window->GetFramebufferWidth(), window->GetFramebufferHeight());

        auto figmentEntity = m_Scene->CreateEntity("Figment");
        figmentEntity.AddComponent<FigmentComponent>();

        auto quad = m_Scene->CreateEntity("Quad");
        quad.AddComponent<QuadComponent>();

        auto cameraEntity = m_Scene->CreateEntity("Camera");
        cameraEntity.AddComponent<CameraComponent>(std::make_shared<PerspectiveCamera>(width / height));

        SelectEntity(figmentEntity);
    }

    EditorLayer::~EditorLayer()
    {
    }

    void EditorLayer::OnAttach()
    {
    }

    void EditorLayer::OnDetach()
    {
    }

    void EditorLayer::OnUpdate(float deltaTime)
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
            glm::vec3 p = m_Scene->GetActiveCameraController()->GetCamera()->ScreenToWorldSpace(
                    Input::GetMousePosition(),
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

    void EditorLayer::DrawScenePanel(const std::vector<Entity> &entities,
            const std::function<void(Entity)> &selectEntity)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
        ImGui::Begin("Scene");

        if (ImGui::Button("Add Entity"))
            m_Scene->CreateEntity("New");

        ImGui::Separator();

        for (auto entity : entities)
        {
            ImGui::PushID((int)entity.GetHandle());
            auto &info = entity.GetComponent<InfoComponent>();
            auto &id = entity.GetComponent<IdComponent>();
            if (ImGui::Selectable(info.m_Name.c_str()))
            {
                selectEntity(entity);
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("UUID: %llu", (uint64_t)id.UUID);
                ImGui::EndTooltip();
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

    static void DrawFigmentComponent(FigmentComponent &figment)
    {
        ImGui::Text("Figment\n%s", figment.ComputeShader->GetShaderSource().c_str());
        ImGui::ColorEdit4("Color", (float *)&figment.Color);
        ImGui::Button("Edit source", ImVec2(100, 20));
        ImGui::SameLine();
        if (!figment.Initialized)
        {
            if (ImGui::Button("Initialize", ImVec2(100, 20)))
            {
            }
        }
        // ImGui::Separator();
        // if (figment.Data != nullptr)
        // {
        //     for (int i = 0; i < figment.Result->GetSize() / sizeof(glm::vec4); i++)
        //     {
        //         ImGui::Text("%f %f %f", figment.Data[i].x, figment.Data[i].y, figment.Data[i].z);
        //     }
        // }
    }

    static void DrawCameraComponent(CameraComponent &camera, Scene &scene)
    {
        bool isActive = scene.GetActiveCameraController() == camera.Controller;
        const char *label = isActive ? "Disable" : "Set Active";
        if (ImGui::SmallButton(label))
        {
            if (isActive)
            {
                scene.SetActiveCameraController(scene.GetEditorCameraController());
            }
            else
            {
                scene.SetActiveCameraController(camera.Controller);
            }
        }
        ImGui::SameLine();
        ImGui::SmallButton("Remove");

        ImGui::Separator();
        ImGui::Text("Aspect: %.2f", camera.Controller->GetCamera()->GetSettings().AspectRatio);
        ImGui::Text("Near: %.2f", camera.Controller->GetCamera()->GetSettings().NearClip);
        ImGui::DragFloat3("Position", (float *)camera.Controller->GetCamera()->GetPositionPtr(), 0.1f, 0.0f, 0.0f,
                "%.2f");

    }

    template<typename T, typename DrawFunction>
    static void DrawComponent(const std::string &name, Entity entity, DrawFunction drawFunction)
    {
        if (entity.HasComponent<T>())
        {
            ImGui::Separator();
            auto &component = entity.GetComponent<T>();
            ImGui::Text("%s", name.c_str());
            float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight * 0.5f);
            bool removeComponent = false;
            if (ImGui::Button("X", ImVec2(lineHeight, lineHeight)))
                removeComponent = true;

            drawFunction(component);

            if (removeComponent)
            {
                entity.RemoveComponent<T>();
            }

        }
    }

    void EditorLayer::DrawEntityInspectorPanel(Entity entity)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 300), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
        ImGui::Begin("Entity Inspector");

        if (!entity)
        {
            ImGui::End();
            return;
        }

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("#addcomponent");

        if (ImGui::BeginPopup("#addcomponent"))
        {
            DisplayAddComponentEntry<TransformComponent>("Transform");
            DisplayAddComponentEntry<CameraComponent>("Camera");
            DisplayAddComponentEntry<QuadComponent>("Quad");
            DisplayAddComponentEntry<CircleComponent>("Circle");
            DisplayAddComponentEntry<FigmentComponent>("Figment");
            ImGui::EndPopup();
        }

        DrawComponent<InfoComponent>("Info", entity, DrawInfoComponent);
        DrawComponent<TransformComponent>("Transform", entity, DrawTransformComponent);
        DrawComponent<QuadComponent>("Quad", entity, DrawQuadComponent);
        DrawComponent<CircleComponent>("Circle", entity, DrawCircleComponent);
        DrawComponent<FigmentComponent>("Figment", entity, [this](auto &component)
        {
            ImGui::ColorEdit4("Color", (float *)&component.Color);
            ImGui::InputInt("Count", &component.Config.Count);
            if (!component.Initialized)
            {
                if (ImGui::Button("Initialize", ImVec2(100, 20)))
                {
                    component.Init(m_Context->GetDevice());
                }
            }
            static bool editSource = false;
            if (ImGui::Button("Edit source", ImVec2(100, 20)))
                editSource = true;

            if (editSource)
            {
                ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Once);
                ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - 600) * 0.5f,
                        (ImGui::GetIO().DisplaySize.y - 400) * 0.5f), ImGuiCond_Once);
                ImGui::Begin("Compute Shader Editor");
                if (ImGui::Button("Save", ImVec2(100, 20)))
                {
                    component.UpdateComputeShader(m_Context->GetDevice());
                    editSource = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Close", ImVec2(100, 20)))
                    editSource = false;
                ImGui::InputTextMultiline("##ComputeCode", component.Config.ComputeShaderSourceBuffer,
                        FigmentComponent::MaxShaderSourceSize,
                        ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiInputTextFlags_AllowTabInput, nullptr, nullptr);
                ImGui::End();
            }
        });

        DrawComponent<CameraComponent>("Camera", entity, [this](auto &component)
        {
            DrawCameraComponent(component, *m_Scene);
        });

        ImGui::End();
    }

    static void DrawActiveCameraSection(CameraController &cameraController)
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

        auto cameraController = scene.GetActiveCameraController();
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
        bool isEditorCamera = scene.GetActiveCameraController() == scene.GetEditorCameraController();
        const char *activeCameraSectionLabel = isEditorCamera ? "Active Camera (Editor)" : "Active Camera (Entity)";
        if (ImGui::TreeNode(activeCameraSectionLabel))
        {
            DrawActiveCameraSection(*scene.GetActiveCameraController());
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

    void EditorLayer::OnImGuiRender()
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
            m_Scene->OnResize(ev->Width, ev->Height);
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

    template<typename T>
    void EditorLayer::DisplayAddComponentEntry(const std::string &entryName)
    {
        if (!m_SelectedEntity.HasComponent<T>())
        {
            if (ImGui::MenuItem(entryName.c_str()))
            {
                m_SelectedEntity.AddComponent<T>();
                ImGui::CloseCurrentPopup();
            }
        }
    }
}


