#include "App.h"
#include <string>

int main()
{
	App *app = new App(1280, 720);
    delete app;
    return 0;
}
