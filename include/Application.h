#pragma once

#include "GLContext.h"
#include "Shader.h"
#include "Camera.h"
#include "GLRenderer.h"
#include "GLObject.h"

class Application
{
private:
	Shader *basicShader;
	// Shader *vertexColorShader;
	// Shader *billboardShader;

	float lastTime;
	float deltaTime;
	double lastUpdateFpsTime;
	int frameCount = 0;

	GLRenderer renderer;

	GLObject plane = GLObject::Plane();
	GLObject gp = GLObject::Triangle();
	GLObject grid = GLObject::Grid(20, 20);
	GLObject wAxis = GLObject::Axis();
	GLObject eAxis = GLObject::Axis();

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
	void RenderFrame();
};
