#include "App.h"
#include "Scene.h"
#include "Entity.h"

#include <math.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec4.hpp>
#include <GLFW/glfw3.h>

App::App(float width, float height)
{
    m_Window = Window::Create("Figment C++", width, height);
    GLFWwindow *glfwWindow = (GLFWwindow *)m_Window->GetNative();
    // m_GLCtx = new GLContext("Figment C++", width, height);
    m_GUICtx = new GUIContext();

    m_GUICtx->Init(glfwWindow, "#version 330 core");

    glfwSetWindowUserPointer(glfwWindow, this);

    auto keyCallback = [](GLFWwindow *w, int key, int scancode, int action, int mods)
    {
        static_cast<App *>(glfwGetWindowUserPointer(w))->HandleKeyboardInput(key, scancode, action, mods);
    };

    glfwSetKeyCallback(glfwWindow, keyCallback);

    auto mouseButtonCallback = [](GLFWwindow *w, int button, int action, int mods)
    {
        static_cast<App *>(glfwGetWindowUserPointer(w))->HandleMouseInput(button, action, mods);
    };

    glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);

    auto mouseCursorCallback = [](GLFWwindow *w, double x, double y)
    {
        static_cast<App *>(glfwGetWindowUserPointer(w))->SetMousePosition(x, y);
    };
    glfwSetCursorPosCallback(glfwWindow, mouseCursorCallback);

    auto mouseScrollCallback = [](GLFWwindow *w, double xOffset, double yOffset)
    {
        static_cast<App *>(glfwGetWindowUserPointer(w))->HandleMouseScroll(xOffset, yOffset);
    };
    glfwSetScrollCallback(glfwWindow, mouseScrollCallback);

    m_Scene = new Scene(m_Window->GetWidth(), m_Window->GetHeight());
    m_Scene->CreateEntity();

    while (!glfwWindowShouldClose(glfwWindow) && glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        Update();
    }
}

App::~App()
{
    delete m_GUICtx;
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

void App::HandleKeyboardInput(int key, int scancode, int action, int mods)
{

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
    {
        ImGui_ImplGlfw_KeyCallback((GLFWwindow *)m_Window->GetNative(), key, scancode, action, mods);
        return;
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        InsertPlane();
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        InsertCircle();
    }
    if (key == GLFW_KEY_I)
    {
        Entity e = m_Scene->CreateEntity("New");
        auto &t = e.GetComponent<TransformComponent>();
        glm::vec3 p = m_Scene->GetCamera()->ScreenToWorldSpace(m_MousePosition);
        t.Position = p;
        auto &b = e.AddComponent<VerletBodyComponent>();
        b.m_PreviousPosition = t.Position;
        b.m_PreviousPosition.x -= 0.5;
        b.m_PreviousPosition.y += 0.1;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        m_FpsCamera = !m_FpsCamera;
        if (m_FpsCamera)
            glfwSetInputMode((GLFWwindow *)m_Window->GetNative(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode((GLFWwindow *)m_Window->GetNative(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void App::HandleMouseInput(int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback((GLFWwindow *)m_Window->GetNative(), button, action, mods);

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        return;
    }

    if (action == 1)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            SelectEntity({(uint32_t)m_Scene->m_HoveredId, m_Scene});
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (m_Scene->m_HoveredId > -1)
            {
                Entity hoveredEntity = {(uint32_t)m_Scene->m_HoveredId, m_Scene};
                if (m_SelectedEntity == hoveredEntity)
                {
                    SelectEntity({});
                }
                m_Scene->DestroyEntity(hoveredEntity);
            }
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            m_Scene->GetCamera()->BeginPan(m_MousePosition);
        }
    }
    if (action == 0)
    {
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            m_Scene->GetCamera()->EndPan();
        }
    }
}

void App::SetMousePosition(double x, double y)
{
    ImGui_ImplGlfw_CursorPosCallback((GLFWwindow *)m_Window->GetNative(), x, y);
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        return;
    }
    this->m_MousePosition.x = x;
    this->m_MousePosition.y = y;
}

void App::HandleMouseScroll(double xOffset, double yOffset)
{
    ImGui_ImplGlfw_ScrollCallback((GLFWwindow *)m_Window->GetNative(), xOffset, yOffset);
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        return;
    }

    m_Scene->GetCamera()->Zoom(yOffset, m_MousePosition);
}

void App::Update()
{
    m_CurrentTime = glfwGetTime();
    double deltaTime = m_CurrentTime - m_LastTime;
    m_LastTime = m_CurrentTime;

    if (glfwGetKey((GLFWwindow *)m_Window->GetNative(), GLFW_KEY_W))
    {
        m_Scene->GetCamera()->Move(CameraDirection::Forward, deltaTime);
    }
    if (glfwGetKey((GLFWwindow *)m_Window->GetNative(), GLFW_KEY_S))
    {
        m_Scene->GetCamera()->Move(CameraDirection::Backward, deltaTime);
    }
    if (glfwGetKey((GLFWwindow *)m_Window->GetNative(), GLFW_KEY_A))
    {
        m_Scene->GetCamera()->Move(CameraDirection::Left, deltaTime);
    }
    if (glfwGetKey((GLFWwindow *)m_Window->GetNative(), GLFW_KEY_D))
    {
        m_Scene->GetCamera()->Move(CameraDirection::Right, deltaTime);
    }

    if (m_FpsCamera)
    {
    }

    GUIUpdate();

    glfwMakeContextCurrent((GLFWwindow *)m_Window->GetNative());
    m_Scene->Update(deltaTime, m_MousePosition);

    m_GUICtx->Render();

    glfwSwapBuffers((GLFWwindow *)m_Window->GetNative());
    glfwPollEvents();
}

void App::GUIUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowSize((GLFWwindow *)m_Window->GetNative(), &windowWidth, &windowHeight);

    size_t cameraWindowWidth = 400;
    ImGui::SetNextWindowPos(ImVec2(windowWidth / 2 - cameraWindowWidth / 2, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(cameraWindowWidth, 0), ImGuiCond_Once);
    ImGui::Begin("Camera");
    glm::vec3 cameraPosition = m_Scene->GetCamera()->GetPosition();
    ImGui::Text("Position x %f, y %f, z %f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
    ImGui::Text("Zoom: %f", m_Scene->GetCamera()->GetZoom());
    ImGui::Text("Aspect: %f", m_Scene->GetCamera()->GetAspectRatio());
    ImGui::Spacing();
    if (ImGui::SmallButton("Reset"))
    {
        m_Scene->GetCamera()->SetPosition(glm::vec3(0.0));
        m_Scene->GetCamera()->SetZoom(1.0);
    }
    if (ImGui::SmallButton("Move Left"))
    {
        cameraPosition.x -= 0.1 * m_Scene->GetCamera()->GetZoom();
        m_Scene->GetCamera()->SetPosition(cameraPosition);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Move Right"))
    {
        cameraPosition.x += 0.1 * m_Scene->GetCamera()->GetZoom();
        m_Scene->GetCamera()->SetPosition(cameraPosition);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Move Up"))
    {
        cameraPosition.y += 0.1 * m_Scene->GetCamera()->GetZoom();
        m_Scene->GetCamera()->SetPosition(cameraPosition);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Move Down"))
    {
        cameraPosition.y -= 0.1 * m_Scene->GetCamera()->GetZoom();
        m_Scene->GetCamera()->SetPosition(cameraPosition);
    }
    ImGui::End();

    const GLubyte *version = glGetString(GL_VERSION);

    glm::vec2 ndc = glm::vec2((m_MousePosition.x / ((float)windowWidth * 0.5)) - 1.0, (m_MousePosition.y / ((float)windowHeight * 0.5)) - 1.0);
    glm::vec2 mw = m_Scene->GetCamera()->ScreenToWorldSpace(m_MousePosition);

    ImGui::SetNextWindowPos(ImVec2(windowWidth - 500, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(500, 0), ImGuiCond_Once);
    ImGui::Begin("Debug");
    ImGui::Text("GL version: %s", version);
    ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    ImGui::Text("GL Vendor: %s", glGetString(GL_VENDOR));
    ImGui::Text("GL Renderer: %s", glGetString(GL_RENDERER));
    ImGui::Separator();

    ImGui::Text("Window size %d %d", windowWidth, windowHeight);

    ImGui::ColorEdit4("clear color", (float *)&m_Scene->m_ClearColor.x);

    if (ImGui::BeginListBox("Mouse"))
    {
        ImGui::Text("Screen: %.2f %.2f", m_MousePosition.x, m_MousePosition.y);
        ImGui::Text("NDC: %.2f %.2f", ndc.x, ndc.y);
        ImGui::Text("World: %.2f %.2f", mw.x, mw.y);
        ImGui::EndListBox();
    }
    ImGui::Text("Hovered entt: %d", m_Scene->m_HoveredId);

    ImGui::End();

    auto entities = m_Scene->GetEntities();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
    ImGui::Begin("Entities");
    for (auto entity : m_Scene->GetEntities())
    {
        auto &id = entity.GetComponent<IDComponent>();
        auto &info = entity.GetComponent<InfoComponent>();
        ImGui::PushID(id.ID);
        char buf[128];
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%-20s (%llu)", info.m_Name.c_str(), id.ID);
        if (ImGui::Selectable(buf))
        {
            SelectEntity(entity);
        }
        ImGui::PopID();
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(0, 300), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
    ImGui::Begin("Inspector");
    if (m_SelectedEntity)
    {
        auto &id = m_SelectedEntity.GetComponent<IDComponent>();
        auto &info = m_SelectedEntity.GetComponent<InfoComponent>();

        char buf[128];
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%s", info.m_Name.c_str());
        if (ImGui::InputText("##Name", buf, sizeof(buf)))
        {
            info.m_Name = std::string(buf);
        }

        auto &transform = m_SelectedEntity.GetComponent<TransformComponent>();
        ImGui::DragFloat3("Position", (float *)&transform.Position.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat3("Rotation", (float *)&transform.Rotation.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat3("Scale", (float *)&transform.Scale.x, 0.1f, 0.0f, 0.0f, "%.2f");
        if (m_SelectedEntity.HasComponent<VerletBodyComponent>())
        {
            auto &body = m_SelectedEntity.GetComponent<VerletBodyComponent>();
            ImGui::Text("Verlet Body");
            ImGui::DragFloat3("Previous position", (float *)&body.m_PreviousPosition.x);
            ImGui::Text("Velocity: x %f y %f z %f", body.m_Velocity.x, body.m_Velocity.y, body.m_Velocity.z);
        }
    }
    ImGui::End();
}

void App::OnResize(float width, float height)
{
    m_Scene->OnResize(width, height);
}

void App::UpdateShader(const char *vertSource, const char *fragSource)
{
    printf("App::UpdateShader -- Not implemented!\n");
}

void App::SelectEntity(Entity entity)
{
    m_SelectedEntity = entity;
}
