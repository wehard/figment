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

const GLchar *vertexSource =
	"#version 300 es\n"
	"layout(location = 0) in vec4 position;    \n"
	"uniform mat4 model_matrix;\n"
	"uniform mat4 view_matrix;\n"
	"uniform mat4 proj_matrix;\n"
	"uniform vec4 obj_color;\n"
	"out vec4 o_col;\n"
	"void main()                  \n"
	"{                            \n"
	"	o_col = obj_color;\n"
	"   gl_Position = proj_matrix * view_matrix * model_matrix * vec4(position.xyz, 1.0);  \n"
	"}                            \n";
const GLchar *fragmentSource =
	"#version 300 es\n"
	"precision mediump float;\n"
	"uniform vec4 obj_color;\n"
	"in vec4 o_col;\n"
	"out vec4 color;\n"
	"void main()                                  \n"
	"{                                            \n"
	"  color = o_col;\n"
	"}                                            \n";

class App
{
private:
	GLContext *gl;
	GUIContext *gui;
	GLRenderer *renderer;
	Camera *camera;
	Shader *shader;
	GLObject *grid;

	ImVec4 clear_color = ImVec4(0.15, 0.15, 0.15, 1.00f);

	std::vector<GLObject *> objects;

public:
	App()
	{
		gl = new GLContext("Figment C++", 1280, 720);
		gui = new GUIContext();
		shader = new Shader(vertexSource, fragmentSource);
		renderer = new GLRenderer();
		camera = new Camera();

		grid = new GLObject(GLObject::Grid(10, 10));
		grid->scale = glm::vec3(500.0);
		camera->Reset(glm::vec3(0.0, 0.0, 10.0), -90.0f, 0.0f);
		gui->Init(gl->window, gl->glContext, gl->glslVersion);
	}

	~App()
	{
		delete gl;
		delete shader;
		delete camera;
		delete gui;
	}

	void Update()
	{
		ImGuiIO &io = ImGui::GetIO();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (io.WantCaptureMouse)
			{
				continue;
			}

			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_i)
			{
				GLObject *o = new GLObject(GLObject::Plane());
				o->position = camera->position;
				o->position.z = 0.0;
				o->color.a = 0.1;
				o->scale = glm::vec3(500.0);
				objects.push_back(o);
			}
			if (event.type == SDL_MOUSEWHEEL)
			{
				float delta = event.wheel.y;
				camera->Scale(delta);
			}
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				GLObject *o = new GLObject(GLObject::Plane());
				o->position = camera->position + camera->forward;
				o->position.z = 0.0;
				o->color.a = 0.1;
				o->scale = glm::vec3(500.0);
				objects.push_back(o);
			}
			if (event.type == SDL_MOUSEBUTTONUP)
			{
			}
		}

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
		ImGui::SetNextWindowSize(ImVec2(500, 120));
		ImGui::Begin("Camera");
		ImGui::Text("Position x %f, y %f, z %f", camera->position.x, camera->position.y, camera->position.z);
		ImGui::Text("Yaw %f, Pitch %f", camera->yaw, camera->pitch);
		if (ImGui::SmallButton("TopLeft"))
		{
			camera->Reset(glm::vec3(-1.0, 2.0, 10.0), -65.0f, -40.0f);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Front"))
		{
			camera->Reset(glm::vec3(0.0, 0.0, 10.0), -90.0f, 0.0f);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Back"))
		{
			camera->Reset(glm::vec3(0.0, 0.0, -10.0), 90.0f, 0.0f);
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
			camera->Move(LEFT, 10.0);
			printf("move left\n");
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Right"))
		{
			camera->Move(RIGHT, 10.0);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Up"))
		{
			camera->Move(UP, 10.0);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Down"))
		{
			camera->Move(DOWN, 10.0);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Forward"))
		{
			camera->Move(FORWARD, 10.0);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Move Back"))
		{
			camera->Move(BACKWARD, 10.0);
		}
		ImGui::End();

		GLint major;
		GLint minor;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &minor);

		ImGui::SetNextWindowPos(ImVec2(1280 - 500, 0));
		ImGui::SetNextWindowSize(ImVec2(500, 120));
		ImGui::Begin("Debug");
		ImGui::Text("GL version: %d.%d", major, minor);
		ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		ImGui::Text("GL Vendor: %s", glGetString(GL_VENDOR));
		ImGui::Text("GL Renderer: %s", glGetString(GL_RENDERER));
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
