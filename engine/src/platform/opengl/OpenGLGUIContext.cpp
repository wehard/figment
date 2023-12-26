#include "OpenGLGUIContext.h"
#include <string>
#include <algorithm>

OpenGLGUIContext::OpenGLGUIContext() {}

void OpenGLGUIContext::Init(std::shared_ptr<Window> window, const char *glslVersion)
{
	IMGUI_CHECKVERSION();
	m_Context = ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

    auto *glfwWindow = (GLFWwindow*)window->GetNative();
	ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
	ImGui_ImplOpenGL3_Init(glslVersion);
}

void OpenGLGUIContext::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGLGUIContext::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(m_Context);
}
