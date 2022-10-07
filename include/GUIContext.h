#pragma once

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "GLContext.h"

struct ApplicationInfo
{
};

class GUIContext
{
private:
	ImGuiContext *context;

public:
	GUIContext();
	void Init(SDL_Window *window, SDL_GLContext sdlContext, const char *glslVersion);
	void Render();
	void Shutdown();
	~GUIContext();
};
