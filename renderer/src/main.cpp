#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "App.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/vec3.hpp>

#include <math.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>

// glm::vec2 mousePosition = glm::vec2(0.0);
// glm::vec2 prevMousePosition = glm::vec2(640, 360);

App *app;

#ifdef __EMSCRIPTEN__
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
		case 3:
			app->InsertCube();
			break;
		default:
			break;
		}
	}

	EMSCRIPTEN_KEEPALIVE void onCanvasResize(float width, float height)
	{
		glViewport(0, 0, width, height);
		app->OnResize(width, height);
	}

	EMSCRIPTEN_KEEPALIVE void setInputEnabled(int enabled)
	{
	}

	EMSCRIPTEN_KEEPALIVE void updateShader(const char *vertSource, const char *fragSource)
	{
		app->UpdateShader(vertSource, fragSource);
	}

	EMSCRIPTEN_KEEPALIVE const char *getDefaultVertShaderSource()
	{
		return Utils::LoadFile("shaders/circle.vert").c_str();
	}

	EMSCRIPTEN_KEEPALIVE const char *getDefaultFragShaderSource()
	{
		return Utils::LoadFile("shaders/circle.frag").c_str();
	}
}
#endif

static void main_loop(void *arg)
{
	App *app = (App *)arg;
	app->Update();
}

int main(int argc, char **argv)
{
	// int width = atoi(argv[1]);
	// int height = atoi(argv[2]);

	// printf("%s:%d Initial canvas size %d x %d\n", __FILE__, __LINE__, width, height);

	app = new App(1920, 1080);

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(main_loop, app, 0, true);
#endif
}
