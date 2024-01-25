#include "Figment.h"

#include <emscripten.h>
#include <cstdio>
#include <string>

using namespace Figment;

class Mesh : public Layer
{
public:
    Mesh() : Layer("Mesh")
    {

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
        ImGui::Begin(this->m_Name.c_str());
        ImGui::End();
    }

    void OnEvent(Figment::AppEvent event, void *eventData) override
    {

    }
};

class Cube : public Layer
{
public:
    Cube() : Layer("Cube")
    {

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
        ImGui::Begin(this->m_Name.c_str());
        ImGui::End();
    }

    void OnEvent(Figment::AppEvent event, void *eventData) override
    {

    }
};

class MainLayer : public Figment::Layer
{
private:
    std::vector<Layer *> m_Layers;
public:
    MainLayer() : Layer("Main")
    {
        m_Layers.push_back(new Cube());
        m_Layers.push_back(new Mesh());

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
        ImGui::Begin("Demos");

        for (auto layer : m_Layers)
        {
            if(ImGui::RadioButton(layer->GetName().c_str(), layer->IsEnabled()))
            {
                layer->SetEnabled(!layer->IsEnabled());
            }
        }
        ImGui::End();
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

    emscripten_set_main_loop_arg(main_loop, app, 0, false);
}
