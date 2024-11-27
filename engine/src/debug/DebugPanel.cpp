#include "DebugPanel.h"
#include "Window.h"
#include "Scene.h"
#include "RenderStats.h"
#include "Input.h"
#include "imgui.h"

namespace figment
{
    void DrawDebugPanel(const Window& window, Camera &camera, bool collapsed)
    {
        glm::vec2 mousePosition = Input::GetMousePosition();
        glm::vec2 ndc = glm::vec2((mousePosition.x / ((float)window.GetWidth() * 0.5)) - 1.0,
                (mousePosition.y / ((float)window.GetHeight() * 0.5)) - 1.0);
        glm::vec3 mw = camera.ScreenToWorldSpace(mousePosition,
                glm::vec2(window.GetWidth(), window.GetHeight()));

        ImGui::SetNextWindowCollapsed(collapsed, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(window.GetWidth() - 400, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Always);
        ImGui::Begin("Debug Info");
        if (ImGui::TreeNodeEx("Window", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("Window", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Viewport");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%dx%d", window.GetWidth(), window.GetHeight());
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Framebuffer");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%dx%d", window.GetFramebufferWidth(), window.GetFramebufferHeight());
                ImGui::EndTable();
            }

            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto isOrthographic = camera.GetType() == CameraType::Orthographic;
            if (ImGui::BeginTable("Camera", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Type");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", isOrthographic ? "Orthographic" : "Perspective");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Aspect ratio");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f", camera.GetAspectRatio());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Position");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f %.2f %.2f", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

                if (isOrthographic)
                {
                    auto orthoCamera = dynamic_cast<OrthographicCamera *>(&camera);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Zoom");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.2f", orthoCamera->GetZoom());
                }

                ImGui::EndTable();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Input", ImGuiTreeNodeFlags_DefaultOpen))
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

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Scroll");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f", Input::GetScrollDelta().y);

                ImGui::EndTable();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Draw calls: %d", RenderStats::DrawCalls);
            ImGui::Text("Vertex count: %d", RenderStats::VertexCount);
            ImGui::Text("Quad count: %d", RenderStats::QuadCount);
            ImGui::Text("Circle count: %d", RenderStats::CircleCount);
            ImGui::Text("Particle count: %d", RenderStats::ParticleCount);
            ImGui::Separator();
            // auto fpsCounter = App::Instance()->GetFPSCounter();
            // ImGui::Text("FPS: %.2f", fpsCounter.GetFPS());
            // ImGui::Text("Frame time: %.2f ms", fpsCounter.GetFrameTime());
            // ImGui::Text("Frame count: %d", fpsCounter.GetFrameCount());
            ImGui::TreePop();
        }
        ImGui::End();
    }
}
