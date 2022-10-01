#pragma once

#include "GLContext.h"
#include "Shader.h"
#include "Camera.h"
#include "GLRenderer.h"

class Application
{
private:
	Shader *particleShader;
	Shader *emitterShader;
	Shader *basicShader;
	Shader *vertexColorShader;
	Shader *billboardShader;

	float lastTime;
	float deltaTime;

	GLRenderer renderer;

public:
	int fps;
	float msPerFrame = 0.0f;
	GLContext &gl;
	bool showOverlays = true;
	bool mouseMovement = false;
	Camera camera;
	MouseInfo mouseInfo;

	Application(GLContext &gl);
	~Application();
	void Run();
};
