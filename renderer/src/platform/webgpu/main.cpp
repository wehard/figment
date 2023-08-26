#include <stdio.h>

#include "App.h"
#include "glm/vec2.hpp"

#include <emscripten.h>
#include <GL/glew.h>
#include <cstdlib>
#include <string>

App *app;

glm::vec2 mousePosition = glm::vec2(0.0);
glm::vec2 prevMousePosition = glm::vec2(640, 360);
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
		glViewport(0, 0, (GLint)width, (GLint)height);
		app->OnResize((uint32_t)width, (uint32_t)height);
        printf("Resize canvas\n");
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
		// return Utils::LoadFile("shaders/circle.vert").c_str();
		return "";
	}

	EMSCRIPTEN_KEEPALIVE const char *getDefaultFragShaderSource()
	{
		// return Utils::LoadFile("shaders/circle.frag").c_str();
		return "";
	}
}

static void main_loop(void *arg)
{
    app->Update();
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Not enough args!\n");
    }
	float width = strtof(argv[1], nullptr);
	float height = strtof(argv[2], nullptr);
	printf("%s:%d Initial canvas size %f x %f\n", __FILE__, __LINE__, width, height);
	app = new App(width, height);

	emscripten_set_main_loop_arg(main_loop, app, 0, false);
}
