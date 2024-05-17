#include "Window.h"
#include "Log.h"
#include <iostream>

int main()
{
    Log::Init();
    auto window = Figment::Window::Create("Figment", 1280, 720);
    while (!window->ShouldClose())
    {
    }
    return 0;
}
