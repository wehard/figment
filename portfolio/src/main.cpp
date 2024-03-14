#include "Figment.h"

#include <emscripten.h>
#include <cstdio>
#include <string>
#include "MainLayer.h"

using namespace Figment;

static void main_loop(void *arg)
{
    App *app = (App *)arg;
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
    App app(width, height);
    MainLayer layer;
    app.AddLayer(&layer);

    emscripten_set_main_loop_arg(main_loop, &app, 0, true);
}
