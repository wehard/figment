#include "App.h"

#include <math.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>

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
    renderer = new GLRenderer();
    camera = new OrthoCamera(width, height);

    grid = new GLObject(GLObject::Grid(10, 10));
    grid->scale = glm::vec3(1.0, 1.0, 1.0);
    grid->color = glm::vec4(1.0, 1.0, 1.0, 0.25);
    plane = new GLObject(GLObject::Plane());
    plane->color = glm::vec4(1.0, 1.0, 1.0, 0.3);
    gui->Init(gl->window, gl->glslVersion);
}

App::~App()
{
    delete gl;
    delete shader;
    delete camera;
    delete gui;
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

// void App::HandleKeyboardInput(SDL_Event event)
// {
//     if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_i)
//     {
//         InsertCircle();
//     }
// }

// void App::HandleMouseInput(SDL_Event event)
// {
//     if (event.type == SDL_MOUSEWHEEL)
//     {
//         float delta = event.wheel.y;
//         camera->Zoom(delta, mousePosition);
//     }
//     if (event.type == SDL_MOUSEBUTTONDOWN)
//     {
//         if (event.button.button == 2)
//         {
//             camera->BeginPan(mousePosition);
//         }
//     }
//     if (event.type == SDL_MOUSEBUTTONUP)
//     {
//         if (event.button.button == 2)
//         {
//             camera->EndPan();
//         }
//     }
// }

void App::Update()
{
    double mx, my;

    mousePosition = glm::vec2(0, 0);
    glfwGetCursorPos(gl->window, &mx, &my);

    ImGuiIO &io = ImGui::GetIO();
    // SDL_Event event;
    // while (m_handleEvents && SDL_PollEvent(&event))
    // {
    //     ImGui_ImplSDL2_ProcessEvent(&event);
    //     if (io.WantCaptureMouse)
    //     {
    //         continue;
    //     }
    //     HandleKeyboardInput(event);
    //     HandleMouseInput(event);

    //     // if (event.window.event == SDL_WINDOWEVENT_RESIZED)
    //     // {
    //     //     OnResize(event.window.data1, event.window.data2);
    //     // }
    // }

    camera->OnUpdate(mousePosition);

    GUIUpdate();

    // SDL_GL_MakeCurrent(gl->window, gl->glContext);
    glfwMakeContextCurrent(gl->window);
    renderer->Begin(*camera, glm::vec4(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w));

    renderer->DrawLines(*grid, *shader);

    for (auto object : objects)
    {
        renderer->Draw(*object, *shader);
    }

    gui->Render();
    // SDL_GL_SwapWindow(gl->window);
    glfwSwapBuffers(gl->window);
    glfwPollEvents();
}

void App::GUIUpdate()
{
    ImGuiIO &io = ImGui::GetIO();

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
    ImGui::SetNextWindowSize(ImVec2(500, 300));
    ImGui::Begin("Debug");
    ImGui::Text("GL version: %s", version);
    ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    ImGui::Text("GL Vendor: %s", glGetString(GL_VENDOR));
    ImGui::Text("GL Renderer: %s", glGetString(GL_RENDERER));
    ImGui::Separator();

    // SDL_GetWindowSize(gl->window, &windowWidth, &windowHeight);
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

    ImGui::End();
}

void App::OnResize(float width, float height)
{
    camera->OnResize(width, height);
    gl->Resize(width, height);
}

void App::SetSDLEventEnabled(bool enabled)
{
    // int state = enabled ? SDL_ENABLE : SDL_DISABLE;
    // SDL_EventState(SDL_TEXTINPUT, state);
    // SDL_EventState(SDL_KEYDOWN, state);
    // SDL_EventState(SDL_KEYUP, state);
    // SDL_EventState(SDL_MOUSEMOTION, state);
    // SDL_EventState(SDL_MOUSEBUTTONDOWN, state);
    // SDL_EventState(SDL_MOUSEBUTTONUP, state);
}
