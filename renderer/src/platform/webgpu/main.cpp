#include "App.h"

#include <emscripten.h>
#include <cstdlib>
#include <stdio.h>
#include <string>

App *app;

static void main_loop(void *arg)
{
    app->Update();
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Not enough args!\n");
    }
	float width = strtof(argv[1], nullptr);
	float height = strtof(argv[2], nullptr);
	printf("%s:%d Initial canvas size %f x %f\n", __FILE__, __LINE__, width, height);
	app = new App(width, height);

	emscripten_set_main_loop_arg(main_loop, app, 0, false);
}
