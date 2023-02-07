#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <emscripten.h>

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

	EMSCRIPTEN_KEEPALIVE void setInputEnabled(int enabled)
	{
		app->SetSDLEventEnabled(enabled);
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
