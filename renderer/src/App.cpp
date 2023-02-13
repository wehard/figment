#include "App.h"

#include <math.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp>

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
    m_FramebufferShader = new Shader(readFile("shaders/framebuffer.vert").c_str(), readFile("shaders/framebuffer.frag").c_str());
    renderer = new GLRenderer();
    camera = new OrthoCamera(width, height);

    grid = new GLObject(GLObject::Grid(10, 10));
    grid->scale = glm::vec3(1.0, 1.0, 1.0);
    grid->color = glm::vec4(1.0, 1.0, 1.0, 0.25);
    plane = new GLObject(GLObject::Plane());
    plane->color = glm::vec4(1.0, 1.0, 1.0, 0.3);
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

    FramebufferDesc desc;
    desc.m_Width = width;
    desc.m_Height = height;
    m_Framebuffer = new Framebuffer(desc);
}

App::~App()
{
    delete gl;
    delete shader;
    delete camera;
    delete gui;
    delete m_Framebuffer;
}

void App::InsertPlane()
{
    GLObject *o = new GLObject(GLObject::Plane());
    o->position = camera->GetPosition();
    o->position.z = 0.0;
    o->color = glm::vec4(1.0, 1.0, 1.0, 1.0);
    o->scale = glm::vec3(0.1, 0.1, 0.0);
    objects.push_back(o);
}

void App::InsertCircle()
{
    GLObject *o = new GLObject(GLObject::Circle(1.0));
    o->position = camera->GetPosition();
    o->position.z = 0.0;
    o->color = glm::vec4(1.0, 1.0, 0.5, 1.0);
    o->scale = glm::vec3(0.1, 0.1, 0.0);
    objects.push_back(o);
}

void App::InsertCube()
{
    GLObject *o = new GLObject(GLObject::Cube());
    o->position = camera->GetPosition();
    o->position.z = 0.0;
    o->color = glm::vec4(0.1, 1.0, 0.2, 1.0);
    o->scale = glm::vec3(0.1, 0.1, 0.0);
    o->rotation = glm::vec3(45.0, 45.0, 0.0);
    objects.push_back(o);
}

void App::HandleKeyboardInput(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_1)
    {
        InsertPlane();
    }
    if (key == GLFW_KEY_2)
    {
        InsertCircle();
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
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            camera->BeginPan(mousePosition);
        }
    }
    if (action == 0)
    {
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            camera->EndPan();
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
    camera->Zoom(yOffset, mousePosition);
}

void App::Update()
{
    camera->OnUpdate(mousePosition);

    GUIUpdate();

    glfwMakeContextCurrent(gl->window);

    renderer->Begin(*camera, glm::vec4(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w));
    m_Framebuffer->Bind();
    m_Framebuffer->ClearAttachment(0, 0);
    m_Framebuffer->ClearAttachment(1, 0);
    renderer->DrawLines(*grid, *shader);

    for (auto object : objects)
    {
        renderer->Draw(*object, *shader);
    }

    m_HoveredId = m_Framebuffer->GetPixel(1, (uint32_t)mousePosition.x, gl->GetHeight() - (uint32_t)mousePosition.y);
    m_Framebuffer->Unbind();
    renderer->DrawTexturedQuad(glm::identity<glm::mat4>(), m_Framebuffer->GetColorAttachmentId(0), *m_FramebufferShader);

    gui->Render();

    glfwSwapBuffers(gl->window);
    glfwPollEvents();
}

void App::GUIUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(500, 200));
    ImGui::Begin("Camera");
    glm::vec3 cameraPosition = camera->GetPosition();
    ImGui::Text("Position x %f, y %f, z %f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
    ImGui::Text("Zoom: %f", camera->GetZoom());
    ImGui::Text("Aspect: %f", camera->GetAspectRatio());
    ImGui::Spacing();
    if (ImGui::SmallButton("Reset"))
    {
        camera->SetPosition(glm::vec3(0.0));
        camera->SetZoom(1.0);
    }
    if (ImGui::SmallButton("Move Left"))
    {
        cameraPosition.x -= 0.1 * camera->GetZoom();
        camera->SetPosition(cameraPosition);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Move Right"))
    {
        cameraPosition.x += 0.1 * camera->GetZoom();
        camera->SetPosition(cameraPosition);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Move Up"))
    {
        cameraPosition.y += 0.1 * camera->GetZoom();
        camera->SetPosition(cameraPosition);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Move Down"))
    {
        cameraPosition.y -= 0.1 * camera->GetZoom();
        camera->SetPosition(cameraPosition);
    }
    ImGui::End();

    const GLubyte *version = glGetString(GL_VERSION);

    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowSize(gl->window, &windowWidth, &windowHeight);

    glm::vec2 ndc = glm::vec2((mousePosition.x / ((float)windowWidth * 0.5)) - 1.0, (mousePosition.y / ((float)windowHeight * 0.5)) - 1.0);
    glm::vec2 mw = camera->ScreenToWorldSpace(mousePosition.x, mousePosition.y);

    ImGui::SetNextWindowPos(ImVec2(windowWidth - 500, 0));
    ImGui::SetNextWindowSize(ImVec2(500, 500));
    ImGui::Begin("Debug");
    ImGui::Text("GL version: %s", version);
    ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    ImGui::Text("GL Vendor: %s", glGetString(GL_VENDOR));
    ImGui::Text("GL Renderer: %s", glGetString(GL_RENDERER));
    ImGui::Separator();

    ImGui::Text("Window size %d %d", windowWidth, windowHeight);

    ImGui::ColorEdit3("clear color", (float *)&m_ClearColor);

    if (ImGui::BeginListBox("Mouse"))
    {
        ImGui::Text("Screen: %.2f %.2f", mousePosition.x, mousePosition.y);
        ImGui::Text("NDC: %.2f %.2f", ndc.x, ndc.y);
        ImGui::Text("World: %.2f %.2f", mw.x, mw.y);
        ImGui::EndListBox();
    }
    ImGui::Text("Objects: %zu", objects.size());
    ImGui::Text("Hovered id: %d", m_HoveredId);

    ImGui::End();
}

void App::OnResize(float width, float height)
{
    m_Framebuffer->Resize(width, height);
    camera->OnResize(width, height);
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
