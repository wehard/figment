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
#include <stdlib.h>
#include <string>

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

glm::vec2 mousePosition = glm::vec2(0.0);
glm::vec2 prevMousePosition = glm::vec2(640, 360);

class App
{
private:
	GLContext *gl;
	GUIContext *gui;
	GLRenderer *renderer;
	OrthoCamera *camera;
	Shader *shader;
	Shader *gridShader;
	GLObject *grid;
	GLObject *plane;

	glm::vec2 mousePosition = glm::vec2(0, 0);
	ImVec4 m_ClearColor = ImVec4(0.15, 0.15, 0.15, 1.00f);

	std::vector<GLObject *> objects;

	// static App *s_Instance;

public:
	App(float width, float height)
	{
		gl = new GLContext("Figment C++", width, height);
		gui = new GUIContext();
		shader = new Shader(readFile("shaders/basic.vert").c_str(), readFile("shaders/basic.frag").c_str());
		gridShader = new Shader(readFile("shaders/grid.vert").c_str(), readFile("shaders/grid.frag").c_str());
		renderer = new GLRenderer();
		camera = new OrthoCamera(width, height);

		grid = new GLObject(GLObject::Grid(10, 10));
		grid->scale = glm::vec3(1.0, 1.0, 1.0);
		grid->color = glm::vec4(1.0, 1.0, 1.0, 0.25);
		plane = new GLObject(GLObject::Plane());
		plane->color = glm::vec4(1.0, 1.0, 1.0, 0.3);
		gui->Init(gl->window, gl->glContext, gl->glslVersion);

		// this->s_Instance = this;
	}

	~App()
	{
		delete gl;
		delete shader;
		delete camera;
		delete gui;
	}

	// static App &Get() { return *s_Instance; }

	void InsertPlane()
	{
		GLObject *o = new GLObject(GLObject::Plane());
		o->position = camera->GetPosition();
		o->position.z = 0.0;
		o->color = glm::vec4(1.0, 1.0, 1.0, 1.0);
		o->scale = glm::vec3(0.1, 0.1, 0.0);
		objects.push_back(o);
	}

	void InsertCircle()
	{
		GLObject *o = new GLObject(GLObject::Circle(1.0));
		o->position = camera->GetPosition();
		o->position.z = 0.0;
		o->color = glm::vec4(1.0, 1.0, 0.5, 1.0);
		o->scale = glm::vec3(0.1, 0.1, 0.0);
		objects.push_back(o);
	}

	void HandleKeyboardInput(SDL_Event event)
	{
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_i)
		{
			InsertCircle();
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

			// if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			// {
			// 	OnResize(event.window.data1, event.window.data2);
			// }
		}

		camera->OnUpdate(mousePosition);

		GUIUpdate();

		SDL_GL_MakeCurrent(gl->window, gl->glContext);
		renderer->Begin(*camera, glm::vec4(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w));

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
		ImGui::Text("Aspect: %f", camera->GetAspectRatio());
		ImGui::Spacing();
		if (ImGui::SmallButton("Reset"))
		{
			camera->SetPosition(glm::vec3(0.0));
			camera->SetZoom(1.0);
		}
		if (ImGui::SmallButton("Move Left"))
		{
			cameraPosition.x -= 0.1 * camera->GetZoom();
			camera->SetPosition(cameraPosition);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Right"))
		{
			cameraPosition.x += 0.1 * camera->GetZoom();
			camera->SetPosition(cameraPosition);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Up"))
		{
			cameraPosition.y += 0.1 * camera->GetZoom();
			camera->SetPosition(cameraPosition);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Down"))
		{
			cameraPosition.y -= 0.1 * camera->GetZoom();
			camera->SetPosition(cameraPosition);
		}
		ImGui::End();

		GLint major;
		GLint minor;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &minor);

		int windowWidth = 0;
		int windowHeight = 0;
		SDL_GetWindowSize(gl->window, &windowWidth, &windowHeight);

		glm::vec2 ndc = glm::vec2((mousePosition.x / ((float)windowWidth * 0.5)) - 1.0, (mousePosition.y / ((float)windowHeight * 0.5)) - 1.0);
		glm::vec2 mw = camera->ScreenToWorldSpace(mousePosition.x, mousePosition.y);

		ImGui::SetNextWindowPos(ImVec2(windowWidth - 500, 0));
		ImGui::SetNextWindowSize(ImVec2(500, 300));
		ImGui::Begin("Debug");
		ImGui::Text("GL version: %d.%d", major, minor);
		ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		ImGui::Text("GL Vendor: %s", glGetString(GL_VENDOR));
		ImGui::Text("GL Renderer: %s", glGetString(GL_RENDERER));
		ImGui::Separator();

		SDL_GetWindowSize(gl->window, &windowWidth, &windowHeight);
		ImGui::Text("Window size %d %d", windowWidth, windowHeight);

		ImGui::ColorEdit3("clear color", (float *)&m_ClearColor);

		if (ImGui::BeginListBox("Mouse"))
		{
			ImGui::Text("Screen: %.2f %.2f", mousePosition.x, mousePosition.y);
			ImGui::Text("NDC: %.2f %.2f", ndc.x, ndc.y);
			ImGui::Text("World: %.2f %.2f", mw.x, mw.y);
			ImGui::EndListBox();
		}
		ImGui::Text("Objects: %zu", objects.size());

		ImGui::End();
	}

	void OnResize(float width, float height)
	{
		camera->OnResize(width, height);
		gl->Resize(width, height);
	}
};

App *app;

extern "C"
{
	EMSCRIPTEN_KEEPALIVE void onMouseMove(float x, float y)
	{
		mousePosition = glm::vec2(x, y);
		glm::vec2 delta = mousePosition - prevMousePosition;
		prevMousePosition = mousePosition;
	}

	EMSCRIPTEN_KEEPALIVE void insertObject(int n)
	{
		printf("JS insert object %d\n", n);
		switch (n)
		{
		case 1:
			app->InsertPlane();
			break;
		case 2:
			app->InsertCircle();
			break;
		default:
			break;
		}
	}

	EMSCRIPTEN_KEEPALIVE void onCanvasResize(float width, float height)
	{
		glViewport(0, 0, width, height);
		app->OnResize(width, height);
		printf("%s Resize to %f x %f\n", __FILE__, width, height);
	}
}

static void main_loop(void *arg)
{
	App *app = (App *)arg;
	app->Update();
}

int main(int argc, char **argv)
{
	int width = atoi(argv[1]);
	int height = atoi(argv[2]);

	printf("%s:%d Initial canvas size %d x %d\n", __FILE__, __LINE__, width, height);

	app = new App(width, height);
	emscripten_set_main_loop_arg(main_loop, app, 0, true);
}
