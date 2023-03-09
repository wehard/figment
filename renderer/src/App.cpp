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

static std::string readFile(std::string path)
{
    std::string source;

    std::ifstream shader_stream(path, std::ios::in);
    if (shader_stream.is_open())
    {
        std::stringstream sstr;
        sstr << shader_stream.rdbuf();
        source = sstr.str();
        shader_stream.close();
    }
    else
        printf("Error opening %s\n", path.c_str());
    return (source);
}

App::App(float width, float height)
{
    gl = new GLContext("Figment C++", width, height);
    gui = new GUIContext();
    shader = new Shader(readFile("shaders/basic.vert").c_str(), readFile("shaders/basic.frag").c_str());
    gridShader = new Shader(readFile("shaders/grid.vert").c_str(), readFile("shaders/grid.frag").c_str());
    m_CircleShader = std::make_unique<Shader>(readFile("shaders/circle.vert").c_str(), readFile("shaders/circle.frag").c_str());

    m_FramebufferShader = new Shader(readFile("shaders/framebuffer.vert").c_str(), readFile("shaders/framebuffer.frag").c_str());

    gui->Init(gl->window, gl->glslVersion);

    glfwSetWindowUserPointer(gl->window, this);

    auto keyCallback = [](GLFWwindow *w, int key, int scancode, int action, int mods)
    {
        static_cast<App *>(glfwGetWindowUserPointer(w))->HandleKeyboardInput(key, scancode, action, mods);
    };

    glfwSetKeyCallback(gl->window, keyCallback);

    auto mouseButtonCallback = [](GLFWwindow *w, int button, int action, int mods)
    {
        static_cast<App *>(glfwGetWindowUserPointer(w))->HandleMouseInput(button, action, mods);
    };

    glfwSetMouseButtonCallback(gl->window, mouseButtonCallback);

    auto mouseCursorCallback = [](GLFWwindow *w, double x, double y)
    {
        static_cast<App *>(glfwGetWindowUserPointer(w))->SetMousePosition(x, y);
    };
    glfwSetCursorPosCallback(gl->window, mouseCursorCallback);

    auto mouseScrollCallback = [](GLFWwindow *w, double xOffset, double yOffset)
    {
        static_cast<App *>(glfwGetWindowUserPointer(w))->HandleMouseScroll(xOffset, yOffset);
    };
    glfwSetScrollCallback(gl->window, mouseScrollCallback);

    m_Scene = new Scene(width, height);
    m_Scene->CreateEntity();
}

App::~App()
{
    delete gl;
    delete shader;
    delete gui;
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
        printf("imgui keyboard capture\n");
        ImGui_ImplGlfw_KeyCallback(gl->window, key, scancode, action, mods);
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
        t.Position = glm::vec3(m_Scene->GetCamera().ScreenToWorldSpace(mousePosition.x, mousePosition.y), 0.0);
        auto &b = e.AddComponent<VerletBodyComponent>();
        b.m_PreviousPosition = t.Position;
        b.m_PreviousPosition.x -= 0.5;
        b.m_PreviousPosition.y += 0.1;
    }
}

void App::HandleMouseInput(int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(gl->window, button, action, mods);

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
            m_Scene->GetCamera().BeginPan(mousePosition);
        }
    }
    if (action == 0)
    {
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            m_Scene->GetCamera().EndPan();
        }
    }
}

void App::SetMousePosition(double x, double y)
{
    ImGui_ImplGlfw_CursorPosCallback(gl->window, x, y);
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        return;
    }
    this->mousePosition.x = x;
    this->mousePosition.y = y;
}

void App::HandleMouseScroll(double xOffset, double yOffset)
{
    ImGui_ImplGlfw_ScrollCallback(gl->window, xOffset, yOffset);
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        return;
    }

    m_Scene->GetCamera().Zoom(yOffset, mousePosition);
}

void App::Update()
{
    m_CurrentTime = glfwGetTime();
    double deltaTime = m_CurrentTime - m_LastTime;
    m_LastTime = m_CurrentTime;

    GUIUpdate();

    glfwMakeContextCurrent(gl->window);
    m_Scene->Update(deltaTime, mousePosition);

    gui->Render();

    glfwSwapBuffers(gl->window);
    glfwPollEvents();
}

void App::GUIUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(600, 0));
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin("Camera");
    glm::vec3 cameraPosition = m_Scene->GetCamera().GetPosition();
    ImGui::Text("Position x %f, y %f, z %f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
    ImGui::Text("Zoom: %f", m_Scene->GetCamera().GetZoom());
    ImGui::Text("Aspect: %f", m_Scene->GetCamera().GetAspectRatio());
    ImGui::Spacing();
    if (ImGui::SmallButton("Reset"))
    {
        m_Scene->GetCamera().SetPosition(glm::vec3(0.0));
        m_Scene->GetCamera().SetZoom(1.0);
    }
    if (ImGui::SmallButton("Move Left"))
    {
        cameraPosition.x -= 0.1 * m_Scene->GetCamera().GetZoom();
        m_Scene->GetCamera().SetPosition(cameraPosition);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Move Right"))
    {
        cameraPosition.x += 0.1 * m_Scene->GetCamera().GetZoom();
        m_Scene->GetCamera().SetPosition(cameraPosition);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Move Up"))
    {
        cameraPosition.y += 0.1 * m_Scene->GetCamera().GetZoom();
        m_Scene->GetCamera().SetPosition(cameraPosition);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Move Down"))
    {
        cameraPosition.y -= 0.1 * m_Scene->GetCamera().GetZoom();
        m_Scene->GetCamera().SetPosition(cameraPosition);
    }
    ImGui::End();

    const GLubyte *version = glGetString(GL_VERSION);

    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowSize(gl->window, &windowWidth, &windowHeight);

    glm::vec2 ndc = glm::vec2((mousePosition.x / ((float)windowWidth * 0.5)) - 1.0, (mousePosition.y / ((float)windowHeight * 0.5)) - 1.0);
    glm::vec2 mw = m_Scene->GetCamera().ScreenToWorldSpace(mousePosition.x, mousePosition.y);

    ImGui::SetNextWindowPos(ImVec2(windowWidth - 500, 0));
    ImGui::SetNextWindowSize(ImVec2(0, 0));
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
        ImGui::Text("Screen: %.2f %.2f", mousePosition.x, mousePosition.y);
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
    gl->Resize(width, height);
}

void App::UpdateShader(const char *vertSource, const char *fragSource)
{
    Shader *newShader = new Shader(vertSource, fragSource);
    if (!newShader->IsValid())
    {
        delete newShader;
        return;
    }

    delete shader;
    shader = newShader;
}

void App::SelectEntity(Entity entity)
{
    m_SelectedEntity = entity;
}
