#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <emscripten.h>

#include "GLContext.h"
#include "GLObject.h"
#include "GUIContext.h"
#include "GLRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/vec3.hpp>

#include <math.h>
#include <fstream>
#include <sstream>

std::string readFile(std::string path)
{
	std::string source;

	std::ifstream shader_stream(path, std::ios::in);
	if (shader_stream.is_open())
	{
		std::stringstream sstr;
		sstr << shader_stream.rdbuf();
		source = sstr.str();
		shader_stream.close();
	}
	else
		printf("Error opening %s\n", path.c_str());
	return (source);
}

class App
{
private:
	GLContext *gl;
	GUIContext *gui;
	GLRenderer *renderer;
	OrthoCamera *camera;
	Shader *shader;
	GLObject *grid;

	glm::vec2 mousePosition = glm::vec2(0, 0);
	ImVec4 clear_color = ImVec4(0.15, 0.15, 0.15, 1.00f);

	std::vector<GLObject *> objects;

public:
	App()
	{
		gl = new GLContext("Figment C++", 1280, 720);
		gui = new GUIContext();
		shader = new Shader(readFile("shaders/basic.vert").c_str(), readFile("shaders/basic.frag").c_str());
		renderer = new GLRenderer();
		camera = new OrthoCamera(1280.0 / 720.0);

		grid = new GLObject(GLObject::Grid(10, 10));
		// grid->scale = glm::vec3(500.0);
		gui->Init(gl->window, gl->glContext, gl->glslVersion);
	}

	~App()
	{
		delete gl;
		delete shader;
		delete camera;
		delete gui;
	}

	void HandleKeyboardInput(SDL_Event event)
	{
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_i)
		{
			GLObject *o = new GLObject(GLObject::Plane());
			o->position = camera->GetPosition();
			o->position.z = 0.0;
			o->color.a = 0.1;
			o->scale = glm::vec3(1.0);
			objects.push_back(o);
		}
	}

	void HandleMouseInput(SDL_Event event)
	{
		if (event.type == SDL_MOUSEWHEEL)
		{
			float delta = event.wheel.y;
			camera->Zoom(delta, mousePosition);
		}
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (event.button.button == 2)
			{
				camera->BeginPan(mousePosition);
			}
		}
		if (event.type == SDL_MOUSEBUTTONUP)
		{
			if (event.button.button == 2)
			{
				camera->EndPan();
			}
		}
	}

	void Update()
	{
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		mousePosition = glm::vec2(mx, my);

		ImGuiIO &io = ImGui::GetIO();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (io.WantCaptureMouse)
			{
				continue;
			}
			HandleKeyboardInput(event);
			HandleMouseInput(event);
		}

		camera->OnUpdate(mousePosition);

		GUIUpdate();

		SDL_GL_MakeCurrent(gl->window, gl->glContext);
		renderer->Begin(*camera, glm::vec4(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
		renderer->DrawLines(*grid, *shader);

		for (auto object : objects)
		{
			renderer->Draw(*object, *shader);
		}

		gui->Render();
		SDL_GL_SwapWindow(gl->window);
	}

	void GUIUpdate()
	{
		ImGuiIO &io = ImGui::GetIO();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(500, 200));
		ImGui::Begin("Camera");
		glm::vec3 cameraPosition = camera->GetPosition();
		ImGui::Text("Position x %f, y %f, z %f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
		ImGui::Text("Zoom: %f", camera->GetZoom());
		ImGui::Spacing();
		if (ImGui::SmallButton("Reset"))
		{
			camera->SetPosition(glm::vec3(0.0));
		}
		if (ImGui::SmallButton("Move Left"))
		{
			cameraPosition.x -= 0.1;
			camera->SetPosition(cameraPosition);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Right"))
		{
			cameraPosition.x += 0.1;
			camera->SetPosition(cameraPosition);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Up"))
		{
			cameraPosition.y += 0.1;
			camera->SetPosition(cameraPosition);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Down"))
		{
			cameraPosition.y -= 0.1;
			camera->SetPosition(cameraPosition);
		}
		ImGui::End();

		GLint major;
		GLint minor;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &minor);

		glm::vec2 ndc = glm::vec2((mousePosition.x / (1280 * 0.5)) - 1.0, (mousePosition.y / (720.0 * 0.5)) - 1.0);
		glm::vec2 mw = camera->ScreenToWorldSpace(mousePosition.x, mousePosition.y);

		ImGui::SetNextWindowPos(ImVec2(1280 - 500, 0));
		ImGui::SetNextWindowSize(ImVec2(500, 200));
		ImGui::Begin("Debug");
		ImGui::Text("GL version: %d.%d", major, minor);
		ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		ImGui::Text("GL Vendor: %s", glGetString(GL_VENDOR));
		ImGui::Text("GL Renderer: %s", glGetString(GL_RENDERER));
		ImGui::Separator();
		ImGui::Text("Mouse position: %.2f %.2f", mousePosition.x, mousePosition.y);
		ImGui::Text("Mouse ndc: %.2f %.2f", ndc.x, ndc.y);
		ImGui::Text("Mouse world: %.2f %.2f", mw.x, mw.y);
		ImGui::Separator();
		ImGui::Text("Objects: %zu", objects.size());

		ImGui::End();
	}
};

glm::vec2 mousePosition = glm::vec2(0.0);
glm::vec2 prevMousePosition = glm::vec2(640, 360);

extern "C"
{

	EMSCRIPTEN_KEEPALIVE void onMouseMove(float x, float y)
	{
		mousePosition = glm::vec2(x, y);
		glm::vec2 delta = mousePosition - prevMousePosition;
		// camera->Rotate(delta.x, delta.y);
		prevMousePosition = mousePosition;
	}

	EMSCRIPTEN_KEEPALIVE void insertObject()
	{
		printf("WebGL insert object\n");
	}
}

static void main_loop(void *arg)
{
	App *app = (App *)arg;
	app->Update();
}

int main(int, char **)
{
	App *app = new App();
	emscripten_set_main_loop_arg(main_loop, app, 0, true);
}