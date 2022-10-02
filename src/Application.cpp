#include "Application.h"
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include "GUIContext.h"

#include <stdio.h>
#include <stdlib.h>

#include <random>
/*
static void glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	auto app = (Application *)glfwGetWindowUserPointer(window);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		printf("Button down");
	}
}

static void glfwMouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	auto app = (Application *)glfwGetWindowUserPointer(window);
}

static void glfwMouseCallback(GLFWwindow *window, double xpos, double ypos)
{
	auto app = (Application *)glfwGetWindowUserPointer(window);

	float offsetX = xpos - app->mouseInfo.screen.x;
	float offsetY = ypos - app->mouseInfo.screen.y;

	glm::vec2 offset = glm::vec2(offsetX, offsetY);

	if (app->mouseMovement)
		app->camera.Rotate(offsetX, offsetY, true);

	app->mouseInfo.screen = glm::vec3(xpos, ypos, 0.0);
	app->mouseInfo.ndc = glm::vec3((float)xpos / ((float)app->gl.width * 0.5f) - 1.0f, (float)ypos / ((float)app->gl.height * 0.5f) - 1.0f, 0.0);
	app->mouseInfo.world = app->gl.GetMouseWorldCoord(&app->camera);
}

static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	auto app = (Application *)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		app->mouseMovement = !app->mouseMovement;
		if (app->mouseMovement)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	// if (key == GLFW_KEY_G && action == GLFW_PRESS)
	// {
	// 	app->mouseInfo.gravity = !app->mouseInfo.gravity;
	// }
}
*/
Application::Application(GLContext &gl) : gl(gl)
{
	basicShader = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
	vertexColorShader = new Shader("res/shaders/vertex_color.vert", "res/shaders/vertex_color.frag");
	billboardShader = new Shader("res/shaders/billboard.vert", "res/shaders/billboard.frag");
	particleShader = new Shader("./res/shaders/particle.vert", "./res/shaders/particle.frag");
	emitterShader = new Shader("./res/shaders/particle_emitter.vert", "./res/shaders/particle_emitter.frag");

	// glfwSetMouseButtonCallback(gl.window, glfwMouseButtonCallback);
	// glfwSetScrollCallback(gl.window, glfwMouseScrollCallback);
	// glfwSetKeyCallback(gl.window, glfwKeyCallback);
	// glfwSetCursorPosCallback(gl.window, glfwMouseCallback);

	camera.Reset(glm::vec3(-1.0, 2.0, 2.0), -65.0f, -40.0f);
}

Application::~Application()
{
	delete particleShader;
	delete emitterShader;
	delete basicShader;
	delete vertexColorShader;
	delete billboardShader;
}

void Application::Run()
{
	GUIContext gui;

	gui.Init(gl.window, gl.glContext, gl.glslVersion);

	gp.color = glm::vec4(0.0, 0.8, 0.8, 1.0);
	gp.scale = glm::vec3(0.05);

	grid.scale = glm::vec3(2.0); // TODO: Needs to be done during vertex creation
	// grid.color = glm::vec4(1.0, 0.0, 0.0, 0.5);
	eAxis.scale = glm::vec3(0.5);

	lastUpdateFpsTime = lastTime;

	bool shouldQuit = false;
	while (!shouldQuit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				shouldQuit = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(gl.window))
				shouldQuit = true;

			if (event.type == SDL_KEYDOWN)
			{
				const Uint8 *state = SDL_GetKeyboardState(nullptr);

				if (state[SDL_SCANCODE_ESCAPE])
				{
					shouldQuit = true;
				}
			}
		}
		double currentTime = 0; // glfwGetTime();
		frameCount++;
		if (currentTime - lastUpdateFpsTime >= 1.0f)
		{
			msPerFrame = 1000.0 / (double)frameCount;
			fps = 1000.0 * (1.0 / msPerFrame);
			frameCount = 0;
			lastUpdateFpsTime = currentTime;
		}
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// if (glfwGetKey(gl.window, GLFW_KEY_W))
		// {
		// 	camera.Move(FORWARD, deltaTime);
		// }
		// if (glfwGetKey(gl.window, GLFW_KEY_S))
		// {
		// 	camera.Move(BACKWARD, deltaTime);
		// }
		// if (glfwGetKey(gl.window, GLFW_KEY_A))
		// {
		// 	camera.Move(LEFT, deltaTime);
		// }
		// if (glfwGetKey(gl.window, GLFW_KEY_D))
		// {
		// 	camera.Move(RIGHT, deltaTime);
		// }

		gui.Update(*this);

		// Render here!
		SDL_GL_MakeCurrent(gl.window, gl.glContext);

		renderer.Begin(camera, gl.clearColor);

		if (showOverlays)
		{
			if (mouseInfo.gravity)
			{
				gp.position = mouseInfo.world;
				renderer.DrawBillboard(gp, 0.05 + (mouseInfo.mass * 0.001), *billboardShader);
			}

			renderer.DrawLines(wAxis, *vertexColorShader);

			basicShader->use();
			basicShader->setVec4("obj_color", grid.color);
			renderer.DrawLines(grid, *basicShader);
		}

		gui.Render();

		SDL_GL_SwapWindow(gl.window);
	}
	gui.Shutdown();
}

void Application::RenderFrame()
{
}
