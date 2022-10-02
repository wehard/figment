#pragma once

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "Application.h"

class GUIContext
{
private:
	ImGuiContext *context;
	void UpdatePlatformInfo(Application &app);
	void UpdateApplicationInfo(Application &app);

public:
	GUIContext();
	void Init(SDL_Window *window, SDL_GLContext sdlContext, const char *glslVersion);
	void Update(Application &app);
	void Render();
	void Shutdown();
	~GUIContext();
};
