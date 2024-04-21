#include "App.h"
#include "EditorLayer.h"

#include <emscripten.h>
#include <cstdio>
#include <string>
#include "WebGPUWindow.h"
#include "Input.h"

using namespace Figment;

Figment::App *app;
Figment::EditorLayer *editorLayer;

extern "C"
{

extern void JS_EntityCreate();

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

    emscripten_set_main_loop_arg(main_loop, app, 0, false);
}
