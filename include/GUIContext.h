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
	// void UpdatePlatformInfo();
	// void UpdateApplicationInfo();

public:
	GUIContext();
	void Init(SDL_Window *window, SDL_GLContext sdlContext, const char *glslVersion);
	void Update(void *arg);
	void Render();
	void Shutdown();
	~GUIContext();
};
