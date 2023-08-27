#include "Core.h"
#include "App.h"
#include "Scene.h"
#include "Entity.h"
#include "Input.h"
#ifndef __EMSCRIPTEN__
#include "ScriptEngine.h"
#endif

#include <string>
#include <GLFW/glfw3.h>

#ifdef FIGMENT_WEB
#include "imgui_impl_wgpu.h"
#endif

App::App(float width, float height)
{
    m_Window = Window::Create("Figment C++", width, height);
    m_Window->SetResizeEventCallback([this](WindowResizeEventData data) {
        OnResize(data.FramebufferWidth, data.FramebufferHeight);
    });

    auto *glfwWindow = (GLFWwindow *)m_Window->GetNative();
    Input::Initialize(glfwWindow);
    m_GUICtx = GUIContext::Create();

#ifdef __EMSCRIPTEN__
    const char *glslVersion = "#version 300 es";
#else
    const char *glslVersion = "#version 330 core";
#endif

    m_GUICtx->Init(m_Window, glslVersion);

//    m_Scene = new Scene(m_Window->GetFramebufferWidth(), m_Window->GetFramebufferHeight());
//    m_Scene->CreateEntity();

#ifndef __EMSCRIPTEN__
    auto scriptEngine = ScriptEngine::Create(ScriptEngineType::Mono, m_Scene);
    scriptEngine->Init();
    while (!m_Window->ShouldClose() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
    {
        Update();
    }
#endif
}

App::~App()
{
    m_GUICtx->Shutdown();
    delete m_Scene;
}

void App::InsertPlane()
{
}

void App::InsertCircle()
{
}

void App::InsertCube()
{
}

void App::HandleKeyboardInput(float deltaTime)
{

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
    {
        return;
    }

    if (Input::GetKeyDown(GLFW_KEY_1))
    {
        InsertPlane();
    }
    if (Input::GetKeyDown(GLFW_KEY_2))
    {
        InsertCircle();
    }
    if (Input::GetKeyDown(GLFW_KEY_I))
    {
        Entity e = m_Scene->CreateEntity("New");
        auto &t = e.GetComponent<TransformComponent>();
        glm::vec3 p = m_Scene->GetCameraController()->GetCamera()->ScreenToWorldSpace(Input::GetMousePosition(), glm::vec2(m_Window->GetFramebufferWidth(), m_Window->GetFramebufferHeight()));
        t.Position = p;
        auto &b = e.AddComponent<VerletBodyComponent>();
        b.m_PreviousPosition = t.Position;
        b.m_PreviousPosition.x -= 0.5;
        b.m_PreviousPosition.y += 0.1;
    }
    if (m_SelectedEntity && (Input::GetKeyDown(GLFW_KEY_DELETE) || Input::GetKeyDown(GLFW_KEY_BACKSPACE)))
    {
        DeleteEntity(m_SelectedEntity);
    }
}

void App::HandleMouseInput()
{
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        return;
    }

    if (Input::GetButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
        SelectEntity({(uint32_t)m_Scene->m_HoveredId, m_Scene});
    }
}

void App::Update()
{
    m_CurrentTime = glfwGetTime();
    double deltaTime = m_CurrentTime - m_LastTime;
    m_LastTime = m_CurrentTime;

//    Input::Update();
//
//    HandleKeyboardInput(deltaTime);
//    HandleMouseInput();

    GUIUpdate();

//    glfwMakeContextCurrent((GLFWwindow *)m_Window->GetNative());
//    m_Scene->Update(deltaTime, Input::GetMousePosition(), glm::vec2(m_Window->GetWidth(), m_Window->GetHeight()));

    m_GUICtx->Render();

//    glfwPollEvents();
//    glfwSwapBuffers((GLFWwindow *)m_Window->GetNative());
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

void App::GUIUpdate()
{
#ifdef FIGMENT_MACOS
    ImGui_ImplOpenGL3_NewFrame();
#elif defined(FIGMENT_WEB)
    ImGui_ImplWGPU_NewFrame();
#endif
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Hello");
    ImGui::Text("World!");
    ImGui::End();
//    auto camera = m_Scene->GetCameraController()->GetCamera();
//    size_t cameraWindowWidth = 400;
//    ImGui::SetNextWindowPos(ImVec2(m_Window->GetWidth() / 2 - cameraWindowWidth / 2, 0), ImGuiCond_Once);
//    ImGui::SetNextWindowSize(ImVec2(cameraWindowWidth, 0), ImGuiCond_Once);
//    ImGui::Begin("Camera");
//    glm::vec3 cameraPosition = camera->GetPosition();
//    ImGui::Text("Position x %f, y %f, z %f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
////    ImGui::Text("Zoom: %f", camera->GetZoom());
//    ImGui::Text("Aspect: %f", camera->GetAspectRatio());
//    ImGui::Spacing();
//    if (ImGui::SmallButton("Reset"))
//    {
//    }
//
//    if (ImGui::DragFloat3("Position", (float *)&cameraPosition.x, 0.1f, 0.0f, 0.0f, "%.2f"))
//    {
//    }
//    ImGui::End();

//    const GLubyte *version = glGetString(GL_VERSION);
//    glm::vec2 mousePosition = Input::GetMousePosition();
//    glm::vec2 ndc = glm::vec2((mousePosition.x / ((float)m_Window->GetWidth() * 0.5)) - 1.0, (mousePosition.y / ((float)m_Window->GetHeight() * 0.5)) - 1.0);
//    glm::vec2 mw = camera->ScreenToWorldSpace(mousePosition, glm::vec2(m_Window->GetWidth(), m_Window->GetHeight()));

//    ImGui::SetNextWindowPos(ImVec2(m_Window->GetWidth() - 500, 0), ImGuiCond_Once);
//    ImGui::SetNextWindowSize(ImVec2(500, 0), ImGuiCond_Once);
//    ImGui::Begin("Info");
//    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
//    if (ImGui::TreeNode("GL"))
//    {
//        ImGui::Text("GL version: %s", version);
//        ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
//        ImGui::Text("GL Vendor: %s", glGetString(GL_VENDOR));
//        ImGui::Text("GL Renderer: %s", glGetString(GL_RENDERER));
//        ImGui::TreePop();
//    }
//
//    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
//    if (ImGui::TreeNode("Window"))
//    {
//        if (ImGui::BeginTable("My Table", 2))
//        {
//            ImGui::TableNextRow();
//            ImGui::TableSetColumnIndex(0);
//            ImGui::Text("Viewport");
//            ImGui::TableSetColumnIndex(1);
//            ImGui::Text("%dx%d", m_Window->GetWidth(), m_Window->GetHeight());
//            ImGui::TableNextRow();
//            ImGui::TableSetColumnIndex(0);
//            ImGui::Text("Framebuffer");
//            ImGui::TableSetColumnIndex(1);
//            ImGui::Text("%dx%d", m_Window->GetFramebufferWidth(), m_Window->GetFramebufferHeight());
//            ImGui::EndTable();
//        }
//
//        ImGui::TreePop();
//    }
//
//    ImGui::ColorEdit4("clear color", (float *)&m_Scene->m_ClearColor.x);
//
//    if (ImGui::BeginListBox("Mouse"))
//    {
//        ImGui::Text("Position: %.2f %.2f", mousePosition.x, mousePosition.y);
//        ImGui::Text("Delta: %.2f %.2f", Input::GetMouseDelta().x, Input::GetMouseDelta().y);
//        ImGui::Text("NDC: %.2f %.2f", ndc.x, ndc.y);
//        ImGui::Text("World: %.2f %.2f", mw.x, mw.y);
//        ImGui::EndListBox();
//    }
//    ImGui::Text("Hovered entt: %d", m_Scene->m_HoveredId);
//
//    ImGui::End();

//    auto entities = m_Scene->GetEntities();

//    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
//    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
//    ImGui::Begin("Entities");
//    for (auto entity : m_Scene->GetEntities())
//    {
//        auto &id = entity.GetComponent<IDComponent>();
//        auto &info = entity.GetComponent<InfoComponent>();
//        ImGui::PushID(id.ID);
//        char buf[128];
//        memset(buf, 0, sizeof(buf));
//        snprintf(buf, sizeof(buf), "%-20s (%llu)", info.m_Name.c_str(), id.ID);
//        if (ImGui::Selectable(buf))
//        {
//            SelectEntity(entity);
//        }
//        ImGui::PopID();
//    }
//    ImGui::End();

//    ImGui::SetNextWindowPos(ImVec2(0, 300), ImGuiCond_Once);
//    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
//    ImGui::Begin("Inspector");
//    if (m_SelectedEntity)
//    {
//        auto &id = m_SelectedEntity.GetComponent<IDComponent>();
//        auto &info = m_SelectedEntity.GetComponent<InfoComponent>();
//
//        char buf[128];
//        memset(buf, 0, sizeof(buf));
//        snprintf(buf, sizeof(buf), "%s", info.m_Name.c_str());
//        if (ImGui::InputText("##Name", buf, sizeof(buf)))
//        {
//            info.m_Name = std::string(buf);
//        }
//
//        auto &transform = m_SelectedEntity.GetComponent<TransformComponent>();
//        ImGui::DragFloat3("Position", (float *)&transform.Position.x, 0.1f, 0.0f, 0.0f, "%.2f");
//        ImGui::DragFloat3("Rotation", (float *)&transform.Rotation.x, 0.1f, 0.0f, 0.0f, "%.2f");
//
//        // ImGui::DragFloat3("Scale", (float *)&transform.Scale.x, 0.1f, 0.0f, 0.0f, "%.2f");
//        static bool syncScale = true;
//        DrawVec3("Scale", &transform.Scale, &syncScale);
//
//        if (m_SelectedEntity.HasComponent<VerletBodyComponent>())
//        {
//            auto &body = m_SelectedEntity.GetComponent<VerletBodyComponent>();
//            ImGui::Text("Verlet Body");
//            ImGui::DragFloat3("Previous position", (float *)&body.m_PreviousPosition.x);
//            ImGui::Text("Velocity: x %f y %f z %f", body.m_Velocity.x, body.m_Velocity.y, body.m_Velocity.z);
//        }
//    }
//    ImGui::End();
}

void App::OnResize(uint32_t width, uint32_t height)
{
    m_Scene->OnResize(width, height);
    Update(); // TODO: Figure out if there is a better way
}

void App::UpdateShader(const char *vertSource, const char *fragSource)
{
    printf("App::UpdateShader -- Not implemented!\n");
}

void App::SelectEntity(Entity entity)
{
    m_SelectedEntity = entity;
}

void App::DeleteEntity(Entity entity)
{
    SelectEntity({});
    m_Scene->DestroyEntity(entity);
}
