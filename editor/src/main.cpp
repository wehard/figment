#include "application.h"
#include "editor_layer.h"

#include "Input.h"
#include <cstdio>
#include <string>

// #include "WebGPUWindow.h"
// #include <emscripten.h>

using namespace figment;

/*
Figment::App *app;
Figment::EditorLayer *editorLayer;
extern "C"
{

extern void JS_EntityCreate();
extern int UserWasmModuleFoo();

EMSCRIPTEN_KEEPALIVE
void enable_input()
{
    app->EnableInput();
}

EMSCRIPTEN_KEEPALIVE
void disable_input()
{
    app->DisableInput();
}

EMSCRIPTEN_KEEPALIVE
Entity entity_get(int handle)
{
    Entity entity = Entity((uint32_t)handle, editorLayer->GetScene().get());
    return entity;
}

EMSCRIPTEN_KEEPALIVE
int entity_create()
{
    Entity entity = editorLayer->GetScene()->CreateEntity();
    return (int)entity.GetHandle();
}

EMSCRIPTEN_KEEPALIVE
void entity_destroy(int handle)
{
    Entity entity = Entity((uint32_t)handle, editorLayer->GetScene().get());
    editorLayer->GetScene()->DestroyEntity(entity);
}

EMSCRIPTEN_KEEPALIVE
void entity_add_component(int handle, int componentType)
{
    Entity entity = Entity((uint32_t)handle, editorLayer->GetScene().get());
    if (componentType == 1)
    {
        entity.AddComponent<QuadComponent>();
    }
    else
    {
        printf("Unknown component type %d\n", componentType);
    }
}

EMSCRIPTEN_KEEPALIVE
void entity_set_name(int handle, const char *name)
{
    Entity entity = Entity((uint32_t)handle, editorLayer->GetScene().get());
    if (entity.HasComponent<InfoComponent>())
    {
        auto &info = entity.GetComponent<InfoComponent>();
        info.m_Name = std::string(name);
    }
    else
    {
        printf("Entity does not have an InfoComponent\n");
    }
}

EMSCRIPTEN_KEEPALIVE
void entity_set_position(int handle, float x, float y, float z)
{
    Entity entity = Entity((uint32_t)handle, editorLayer->GetScene().get());
    if (entity.HasComponent<TransformComponent>())
    {
        auto &transform = entity.GetComponent<TransformComponent>();
        transform.Position = { x, y, z };
    }
    else
    {
        printf("Entity does not have a TransformComponent\n");
    }
}

EMSCRIPTEN_KEEPALIVE
void entity_set_rotation(int handle, float x, float y, float z)
{
    Entity entity = Entity((uint32_t)handle, editorLayer->GetScene().get());
    if (entity.HasComponent<TransformComponent>())
    {
        auto &transform = entity.GetComponent<TransformComponent>();
        transform.Rotation = { x, y, z };
    }
    else
    {
        printf("Entity does not have a TransformComponent\n");
    }
}

EMSCRIPTEN_KEEPALIVE
void entity_set_scale(int handle, float x, float y, float z)
{
    Entity entity = Entity((uint32_t)handle, editorLayer->GetScene().get());
    if (entity.HasComponent<TransformComponent>())
    {
        auto &transform = entity.GetComponent<TransformComponent>();
        transform.Scale = { x, y, z };
    }
    else
    {
        printf("Entity does not have a TransformComponent\n");
    }
}

} // extern "C"

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
    editorLayer = new EditorLayer();
    app->AddLayer(editorLayer);

    // UserWasmModuleFoo();

    free((void *)0);

    emscripten_set_main_loop_arg(main_loop, app, 0, false);
}
*/

int main()
{
    Application app({.Name = "Figment Editor", .Width = 1280, .Height = 720});
    auto window      = app.getWindow();
    auto editorLayer = EditorLayer(window);
    app.addLayer(&editorLayer);
    app.start();
    return 0;
}
