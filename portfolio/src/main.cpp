#include "Figment.h"

#include <emscripten.h>
#include <cstdio>
#include <string>

using namespace Figment;

class Cube : public Figment::Layer
{
public:
    Cube(const std::string &name) : Layer(name)
    {

    }
    void OnAttach()
    {

    }

    void OnDetach()
    {

    }

    void OnUpdate(float deltaTime)
    {

    }

    void OnImGuiRender()
    {
        ImGui::Begin(this->m_Name.c_str());
        ImGui::End();
    }

    void OnEvent(Figment::AppEvent event, void *eventData)
    {

    }
};

class MainLayer : public Figment::Layer
{
private:
    Figment::UniquePtr<Cube> m_Cube;
public:
    MainLayer() : Layer("Main")
    {
        m_Cube = CreateUniquePtr<Cube>("Cube");
        App::Instance()->AddLayer(m_Cube.get());
    }

    void OnAttach()
    {

    }

    void OnDetach()
    {

    }

    void OnUpdate(float deltaTime)
    {

    }

    void OnImGuiRender()
    {
        static bool active = true;
        ImGui::Begin("Demos");
        if(ImGui::RadioButton("Cube", active))
        {
            active = !active;
            m_Cube->SetEnabled(active);
        }
        ImGui::End();
    }

    void OnEvent(Figment::AppEvent event, void *eventData)
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

    emscripten_set_main_loop_arg(main_loop, app, 0, false);
}
