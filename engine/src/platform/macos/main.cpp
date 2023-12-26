#include "App.h"
#include <string>

int main()
{
	auto *app = new Figment::App(1280, 720);
    delete app;
    return 0;
}
