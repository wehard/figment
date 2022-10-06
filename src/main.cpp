#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <emscripten.h>

#include "GLContext.h"
#include "Application.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/vec3.hpp>

const GLchar *vertexSource =
	"attribute vec4 position;    \n"
	"uniform mat4 model_matrix;\n"
	"uniform mat4 view_matrix;\n"
	"uniform mat4 proj_matrix;\n"
	"void main()                  \n"
	"{                            \n"
	"   gl_Position =  proj_matrix * view_matrix * model_matrix * vec4(position.xyz, 1.0);  \n"
	"}                            \n";
const GLchar *fragmentSource =
	"precision mediump float;\n"
	"void main()                                  \n"
	"{                                            \n"
	"  gl_FragColor = vec4 (1.0, 0.0, 1.0, 1.0 );\n"
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

	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
	}

	gui->Update();

	SDL_GL_MakeCurrent(g_Window, g_GLContext);
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	renderer->Begin(*camera, glm::vec4(clear_color.x, clear_color.y, clear_color.z, clear_color.w));

	glDrawArrays(GL_TRIANGLES, 0, 3);

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

	shader = new Shader(vertexSource, fragmentSource);

	renderer = new GLRenderer();
	camera = new Camera();
	camera->Reset(glm::vec3(-1.0, 2.0, 2.0), -65.0f, -40.0f);

	{
		// Create Vertex Array Object
		GLuint vao;
		glGenVertexArraysOES(1, &vao);
		glBindVertexArrayOES(vao);

		// Create a Vertex Buffer Object and copy the vertex data to it
		GLuint vbo;
		glGenBuffers(1, &vbo);

		GLfloat vertices[] = {0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f};

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		/*
		// Create and compile the vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);

		// Create and compile the fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShader);

		// Link the vertex and fragment shader into a shader program
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		// glBindFragDataLocation(shaderProgram, 0, "outColor");
		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);
		*/

		shader->use();

		// Specify the layout of the vertex data
		GLint posAttrib = glGetAttribLocation(shader->p_id, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		shader->setMat4("proj_matrix", camera->getProjectionMatrix());
		shader->setMat4("view_matrix", camera->getViewMatrix());
		shader->setMat4("model_matrix", getModelMatrix());

		// glUniformMatrix4fv(glGetUniformLocation(shader->p_id, "proj_matrix"), 1, GL_FALSE, (GLfloat *)&camera->getProjectionMatrix()[0]);
		// glUniformMatrix4fv(glGetUniformLocation(shader->p_id, "view_matrix"), 1, GL_FALSE, (GLfloat *)&camera->getViewMatrix()[0]);
		// glUniformMatrix4fv(glGetUniformLocation(shader->p_id, "model_matrix"), 1, GL_FALSE, (GLfloat *)&getModelMatrix()[0]);
	}

	emscripten_set_main_loop_arg(main_loop, NULL, 0, true);
}
