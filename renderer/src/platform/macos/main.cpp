#include "App.h"
#include <string>

int main()
{
	App *app = new App(1000, 1000);
    delete app;
    return 0;
}
