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

void GUIContext::Update()
{
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// UpdatePlatformInfo();
	// UpdateApplicationInfo();

	ImGui::Begin("test");
	ImGui::Text("Hello, world!");
	ImGui::End();
}

// void GUIContext::UpdateApplicationInfo()
// {
// 	ImGui::Begin("application");
// 	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
// 	ImGui::Text("FPS %d", app.fps);
// 	ImGui::Text("ms %f", app.msPerFrame);
// 	ImGui::Checkbox("Show Overlays", &app.showOverlays);
// 	ImGui::Separator();

// 	ImGui::Text("Camera");
// 	ImGui::Text("Position x %f, y %f, z %f", app.camera.position.x, app.camera.position.y, app.camera.position.z);
// 	ImGui::Text("Yaw %f, Pitch %f", app.camera.yaw, app.camera.pitch);
// 	if (ImGui::SmallButton("TopLeft"))
// 	{
// 		app.camera.Reset(glm::vec3(-1.0, 2.0, 2.0), -65.0f, -40.0f);
// 	}
// 	ImGui::SameLine();
// 	if (ImGui::SmallButton("Front"))
// 	{
// 		app.camera.Reset(glm::vec3(0.0, 0.0, 2.0), -90.0f, 0.0f);
// 	}
// 	ImGui::SameLine();
// 	if (ImGui::SmallButton("Back"))
// 	{
// 		app.camera.Reset(glm::vec3(0.0, 0.0, -2.0), 90.0f, 0.0f);
// 	}
// 	ImGui::SameLine();
// 	if (ImGui::SmallButton("Left"))
// 	{
// 		app.camera.Reset(glm::vec3(-2.0, 0.0, 0.0), 0.0f, 0.0f);
// 	}
// 	ImGui::SameLine();
// 	if (ImGui::SmallButton("Right"))
// 	{
// 		app.camera.Reset(glm::vec3(2.0, 0.0, 0.0), 180.0f, 0.0f);
// 	}
// 	ImGui::SameLine();
// 	if (ImGui::SmallButton("Top"))
// 	{
// 		app.camera.Reset(glm::vec3(0.0, 2.0, 0.0), -90.0f, -89.0f);
// 	}
// 	ImGui::Separator();

// 	ImGui::Text("Mouse");
// 	ImGui::Text("Screen x %f, y %f, z %f", app.mouseInfo.screen.x, app.mouseInfo.screen.y, app.mouseInfo.screen.z);
// 	ImGui::Text("NDC x %f, y %f, z %f", app.mouseInfo.ndc.x, app.mouseInfo.ndc.y, app.mouseInfo.ndc.z);
// 	ImGui::Text("World x %f, y %f, z %f", app.mouseInfo.world.x, app.mouseInfo.world.y, app.mouseInfo.world.z);
// 	ImGui::Checkbox("Gravity", (bool *)&app.mouseInfo.gravity);
// 	ImGui::SameLine();
// 	ImGui::SliderFloat("Mass", &app.mouseInfo.mass, 1.0, 100.0, "%.2f", ImGuiSliderFlags_None);
// 	ImGui::Separator();

// 	ImGui::Text("Info");
// 	ImGui::ColorEdit3("Background color", &app.gl.clearColor.r);

// 	ImGui::End();
// }

// void GUIContext::UpdatePlatformInfo(Application &app)
// {
// 	ImGui::Begin("platform");
// 	ImGui::Text("GL version: %d.%d", app.gl.glInfo.glMajorVersion, app.gl.glInfo.glMinorVersion);
// 	ImGui::Text("GLSL version: %s", app.gl.glInfo.shadingLanguageVersion);
// 	ImGui::Text("GL Vendor: %s", app.gl.glInfo.vendor);
// 	ImGui::Text("GL Renderer: %s", app.gl.glInfo.renderer);
// 	ImGui::Separator();

// 	ImGui::End();
// }

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
