#pragma once

#include "GUIContext.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

struct ApplicationInfo
{
};

class OpenGLGUIContext : public GUIContext
{
private:
    ImGuiContext *m_Context;

public:
	OpenGLGUIContext();
	void Init(std::shared_ptr<Window> window, const char *glslVersion) override;
	void Render() override;
	void Shutdown() override;
	~OpenGLGUIContext() override = default;
};
