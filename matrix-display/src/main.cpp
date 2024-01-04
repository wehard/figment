#include "App.h"

#include <emscripten.h>
#include <cstdio>
#include <string>
#include "Log.h"
#include "MatrixDisplay.h"

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
    app->AddLayer(Figment::CreateUniquePtr<MatrixDisplay>(64, 32, width, height));

	emscripten_set_main_loop_arg(main_loop, app, 0, false);
}
