#include "App.h"
#include "EditorLayer.h"

#include <emscripten.h>
#include <cstdio>
#include <string>
#include "WebGPUWindow.h"
#include "Input.h"

class MatrixDisplay : public Figment::Layer
{
public:
    MatrixDisplay()
    {
        const auto window = std::dynamic_pointer_cast<Figment::WebGPUWindow>(Figment::App::Instance()->GetWindow());
        auto context = window->GetContext();
        m_Scene = Figment::CreateUniquePtr<Figment::Scene>(window->GetFramebufferWidth(), window->GetFramebufferHeight());


        for (int y = 0; y < m_Height; ++y)
        {
            for (int x = 0; x < m_Width; ++x)
            {
                auto entity = m_Scene->CreateEntity("MatrixDisplay");
                auto &c = entity.AddComponent<Figment::CircleComponent>();
                c.Thickness = 1;
                auto &t = entity.GetComponent<Figment::TransformComponent>();
                t.Position = glm::vec3(x - 32, y - 16, 0);
            }
        }
    }

    void OnAttach() override {}
    void OnDetach() override {}
    void OnUpdate(float deltaTime) override {
        auto m_Window = Figment::App::Instance()->GetWindow();
        m_Scene->OnUpdate(deltaTime, Figment::Input::GetMousePosition(), glm::vec2(m_Window->GetWidth(), m_Window->GetHeight()));
    }
    void OnImGuiRender() override {
        ImGui::Begin("Matrix");
        ImGui::Text("Hello, world!");
        ImGui::End();
    }
    void OnEvent(Figment::AppEvent event, void *eventData) override {}
private:
    uint32_t m_Width = 64;
    uint32_t m_Height = 32;
    Figment::UniquePtr<Figment::Scene> m_Scene;
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
	printf("%s:%d Initial canvas size %u x %u\n", __FILE__, __LINE__, width, height);
	app = new Figment::App(width, height);
    app->AddLayer(Figment::CreateUniquePtr<Figment::EditorLayer>());
    // app->AddLayer(Figment::CreateUniquePtr<MatrixDisplay>());

	emscripten_set_main_loop_arg(main_loop, app, 0, false);
}
