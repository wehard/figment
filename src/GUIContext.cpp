#include "GUIContext.h"
#include <string>
#include <algorithm>

GUIContext::GUIContext() {}

void GUIContext::Init(SDL_Window *window, SDL_GLContext sdlContext, const char *glslVersion)
{
	IMGUI_CHECKVERSION();
	context = ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, sdlContext);
	ImGui_ImplOpenGL3_Init(glslVersion);
}

void GUIContext::Update(void *arg)
{
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// UpdatePlatformInfo();
	// UpdateApplicationInfo();

	Camera *camera = (Camera *)arg;

	ImGui::Begin("Camera");
	ImGui::Text("Position x %f, y %f, z %f", camera->position.x, camera->position.y, camera->position.z);
	ImGui::Text("Yaw %f, Pitch %f", camera->yaw, camera->pitch);
	if (ImGui::SmallButton("TopLeft"))
	{
		camera->Reset(glm::vec3(-1.0, 2.0, 2.0), -65.0f, -40.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Front"))
	{
		camera->Reset(glm::vec3(0.0, 0.0, 2.0), -90.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Back"))
	{
		camera->Reset(glm::vec3(0.0, 0.0, -2.0), 90.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Left"))
	{
		camera->Reset(glm::vec3(-2.0, 0.0, 0.0), 0.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Right"))
	{
		camera->Reset(glm::vec3(2.0, 0.0, 0.0), 180.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Top"))
	{
		camera->Reset(glm::vec3(0.0, 2.0, 0.0), -90.0f, -89.0f);
	}
	ImGui::Text("Move");
	if (ImGui::SmallButton("Move Left"))
	{
		camera->Move(LEFT, 0.1);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Move Right"))
	{
		camera->Move(RIGHT, 0.1);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Move Up"))
	{
		camera->Move(UP, 0.1);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Move Down"))
	{
		camera->Move(DOWN, 0.1);
	}
	ImGui::End();

	GLint major;
	GLint minor;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &minor);

	ImGui::Begin("GL Platform");
	ImGui::Text("GL version: %d.%d", major, minor);
	ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	ImGui::Text("GL Vendor: %s", glGetString(GL_VENDOR));
	ImGui::Text("GL Renderer: %s", glGetString(GL_RENDERER));

	ImGui::End();
}

void GUIContext::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIContext::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext(context);
}

GUIContext::~GUIContext() {}
