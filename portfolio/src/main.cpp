#include "Figment.h"
#include "DebugPanel.h"

#include <emscripten.h>
#include <cstdio>
#include <string>

using namespace Figment;

class Cube : public Layer
{
private:
    UniquePtr<WebGPURenderer> m_Renderer;
    SharedPtr<PerspectiveCamera> m_Camera;
    Figment::Mesh *m_Mesh;
    WebGPUShader *m_Shader;
    glm::vec3 m_Position = glm::vec3(0.0);
    glm::vec3 m_Rotation = glm::vec3(0.0);
    glm::vec3 m_Scale = glm::vec3(1.0);
public:
    Cube(SharedPtr<PerspectiveCamera> camera) : Layer("Cube"), m_Camera(camera)
    {
        auto m_Window = Figment::App::Instance()->GetWindow();
        auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
        m_Renderer = Figment::CreateUniquePtr<Figment::WebGPURenderer>(*webGpuWindow->GetContext());



        std::vector<Vertex> vertices = {
                {{-0.5, -0.5, 0.5}},
                {{0.5, -0.5, 0.5}},
                {{0.5, 0.5, 0.5}},
                {{-0.5, 0.5, 0.5}},
                {{-0.5, -0.5, -0.5}},
                {{0.5, -0.5, -0.5}},
                {{0.5, 0.5, -0.5}},
                {{-0.5, 0.5, -0.5}}
        };

        std::vector<uint32_t> indices = {
                0, 1, 2, 2, 3, 0,
                1, 5, 6, 6, 2, 1,
                7, 6, 5, 5, 4, 7,
                4, 0, 3, 3, 7, 4,
                4, 5, 1, 1, 0, 4,
                3, 2, 6, 6, 7, 3
        };

        m_Mesh = new Figment::Mesh(webGpuWindow->GetContext()->GetDevice(), vertices, indices );
        m_Shader = new WebGPUShader(webGpuWindow->GetContext()->GetDevice(), *Utils::LoadFile2("res/shaders/wgsl/mesh.wgsl"));
    }

    void OnAttach() override
    {

    }

    void OnDetach() override
    {

    }

    void OnUpdate(float deltaTime) override
    {
        m_Rotation.x += 90.0f * deltaTime;
        m_Rotation.y += 90.0f * deltaTime;
        m_Rotation.z += 10.0f * deltaTime;
        m_Camera->Update();
        m_Renderer->Begin(*m_Camera);
        glm::mat4 matScale = glm::scale(glm::mat4(1.0f), m_Scale);
        glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), m_Position);
        glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(m_Rotation.x), glm::radians(m_Rotation.y),
                glm::radians(m_Rotation.z));
        glm::mat4 transform = matTranslate * matRotate * matScale;
        m_Renderer->Submit(*m_Mesh, transform, *m_Shader);
        m_Renderer->End();
    }

    void OnImGuiRender() override
    {
    }

    void OnEvent(Figment::AppEvent event, void *eventData) override
    {
        auto ev = (Figment::WindowResizeEventData *)eventData;
        m_Camera->Resize((float)ev->Width, (float)ev->Height);
        m_Renderer->OnResize(ev->Width, ev->Height);
    }
};

class MainLayer : public Figment::Layer
{
private:
    std::vector<Layer *> m_Layers;
    SharedPtr<PerspectiveCamera> m_Camera;
public:
    MainLayer() : Layer("Main")
    {
        auto m_Window = Figment::App::Instance()->GetWindow();
        auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
        m_Camera = CreateSharedPtr<PerspectiveCamera>((float)webGpuWindow->GetWidth() / (float)webGpuWindow->GetHeight());
        m_Camera->SetPosition(glm::vec3(0.0, 0.0, 3.0));

        m_Layers.push_back(new Cube(m_Camera));

        for (auto layer : m_Layers)
        {
            App::Instance()->AddLayer(layer);
        }
    }

    void OnAttach() override
    {

    }

    void OnDetach() override
    {

    }

    void OnUpdate(float deltaTime) override
    {

    }

    void OnImGuiRender() override
    {
        ImVec2 appWindowSize = ImVec2((float)App::Instance()->GetWindow()->GetWidth(), (float)App::Instance()->GetWindow()->GetHeight());
        const int padding = 10;

        auto width = appWindowSize.x / 5;
        auto height = appWindowSize.y / 3;

        // Make window rounded
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);

        ImGui::SetNextWindowPos(ImVec2(padding, padding), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(width * 2, height), ImGuiCond_FirstUseEver);
        ImGui::Begin("Willehard Korander");
        ImGui::Text("EXPERIENCE");
        ImGui::Text("[OCT 23 - PRESENT]: Senior Software Engineer, Unity Technologies, Helsinki");
        ImGui::Text("[APR 21 - OCT 23]:  Software Engineer, Unity Technologies, Helsinki");
        ImGui::Text("[AUG 20 - APR 21]:  Software Developer Intern, Unity Technologies, Helsinki");
        ImGui::Spacing();
        ImGui::Text("EDUCATION");
        ImGui::Text("[OCT 19 - JUL 20]: Hive Helsinki, Computer Science");
        ImGui::Text("[2018]:            Introduction to databases, University of Helsinki, Open University");
        ImGui::Text("[2016]:            Advanced course in programming, University of Helsinki, Open University");
        ImGui::Text("[2016]:            Introduction to programming, University of Helsinki, Open University");
        ImGui::Text("[AUG 03 - JUL 07]: Theatre Academy, Helsinki, Master of Arts in Acting");
        ImGui::Spacing();
        ImGui::Text("SKILLS");
        ImGui::Text("C, C++, C#, Typescript,\nOpenGL, WebGPU, Vulkan, Unity,\nGit, Linux, macOS, Windows, \nWeb, React, Node.js, GraphQL, REST,\nGCP, Jira, Confluence");

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(appWindowSize.x - width - padding, padding + 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);
        ImGui::Begin("Layers");

        for (auto layer : m_Layers)
        {
            auto enabled = layer->IsEnabled();
            if(ImGui::Checkbox(layer->GetName().c_str(), &enabled))
            {
                layer->SetEnabled(!layer->IsEnabled());
            }
        }
        ImGui::End();

        Figment::DrawDebugPanel(*m_Camera, true);
    }

    void OnEvent(Figment::AppEvent event, void *eventData) override
    {

    }
};

Figment::App *app;

static void main_loop(void *arg)
{
    app->Update();
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Wrong number of arguments\n");
        return 1;
    }
    uint32_t width = std::stoul(argv[1]);
    uint32_t height = std::stoul(argv[2]);
    app = new Figment::App(width, height);
    app->AddLayer(new MainLayer());

    auto model = ModelLoader::LoadGltf("res/box.gltf");

    emscripten_set_main_loop_arg(main_loop, app, 0, false);
}
