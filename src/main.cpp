#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <emscripten.h>

#include "GLContext.h"
#include "Application.h"

// Emscripten requires to have full control over the main loop. We're going to store our SDL book-keeping variables globally.
// Having a single function that acts as a loop prevents us to store state in the stack of said function. So we need some location for this.
SDL_Window *g_Window = NULL;
SDL_GLContext g_GLContext = NULL;

GUIContext *gui;

GLContext *gl = NULL;
Application *app = NULL;

static void main_loop(void *arg)
{
	ImGuiIO &io = ImGui::GetIO();
	IM_UNUSED(arg);

	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
	}

	gui->Update();

	SDL_GL_MakeCurrent(g_Window, g_GLContext);
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	gui->Render();
	SDL_GL_SwapWindow(g_Window);
}

int main(int, char **)
{
	gl = new GLContext("crap canvas", 1920, 1080);
	g_Window = gl->window;
	g_GLContext = gl->glContext;

	gui = new GUIContext();
	gui->Init(g_Window, g_GLContext, gl->glslVersion);

	emscripten_set_main_loop_arg(main_loop, NULL, 0, true);
}
