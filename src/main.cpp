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

// Emscripten requires to have full control over the main loop. We're going to store our SDL book-keeping variables globally.
// Having a single function that acts as a loop prevents us to store state in the stack of said function. So we need some location for this.
SDL_Window *g_Window = NULL;
SDL_GLContext g_GLContext = NULL;

GUIContext *gui;
GLContext *gl = NULL;
GLRenderer *renderer;
Camera *camera;
Shader *shader;
GLObject *triangle;

glm::vec2 mousePosition = glm::vec2(0.0);
glm::vec2 prevMousePosition = glm::vec2(640, 360);

extern "C"
{

	EMSCRIPTEN_KEEPALIVE void onMouseMove(float x, float y)
	{
		mousePosition = glm::vec2(x, y);
		glm::vec2 delta = mousePosition - prevMousePosition;
		camera->Rotate(delta.x, delta.y);
		prevMousePosition = mousePosition;
	}
}

glm::mat4 getModelMatrix()
{
	glm::vec3 rotation = glm::vec3(0.0, 0.0, 0.0);
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0));
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
	glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
	glm::mat4 m = matTranslate * matRotate * matScale;
	return (m);
}

static void main_loop(void *arg)
{
	ImGuiIO &io = ImGui::GetIO();
	IM_UNUSED(arg);

	static ImVec4 clear_color = ImVec4(0.15, 0.15, 0.15, 1.00f);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
	}

	gui->Update(camera);

	SDL_GL_MakeCurrent(g_Window, g_GLContext);
	// glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	renderer->Begin(*camera, glm::vec4(clear_color.x, clear_color.y, clear_color.z, clear_color.w));

	GLObject *grid = (GLObject *)arg;
	renderer->DrawLines(*grid, *shader);

	gui->Render();
	SDL_GL_SwapWindow(g_Window);
}

int main(int, char **)
{
	gl = new GLContext("Figment C++", 1280, 720);
	g_Window = gl->window;
	g_GLContext = gl->glContext;

	gui = new GUIContext();
	gui->Init(g_Window, g_GLContext, gl->glslVersion);

	shader = new Shader(vertexSource, fragmentSource);

	renderer = new GLRenderer();
	camera = new Camera();
	camera->Reset(glm::vec3(0.0, 0.0, 4.0), -90.0f, 0.0f);

	triangle = new GLObject(std::vector<float>{
		-0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f});

	auto grid = GLObject::Grid(10, 10);
	grid.scale = glm::vec3(2.0);
	grid.color = glm::vec4(1.0, 1.0, 1.0, 0.3);

	emscripten_set_main_loop_arg(main_loop, &grid, 0, true);
}
