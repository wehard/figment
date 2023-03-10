#pragma once

#define GL_GLEXT_PROTOTYPES 1
// #include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
// #include <SDL_opengles2.h>
// #include <SDL2/SDL_opengl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#else
// #include <SDL2/SDL_opengl.h>
// #include <SDL2/SDL_opengles2.h>
#endif

#include <glm/gtc/matrix_transform.hpp>
#include <string>

struct GLInfo
{
	GLint glMajorVersion;
	GLint glMinorVersion;
	GLint glslMajorVersion;
	GLint glslMinorVersion;
	const GLubyte *shadingLanguageVersion;
	const GLubyte *vendor;
	const GLubyte *renderer;
};

struct MouseInfo
{
	glm::vec3 screen;
	glm::vec3 ndc;
	glm::vec3 world;
	bool gravity = false;
	float mass = 1.0f;
};

class GLContext
{
private:
	void readGLInfo();
	int m_Width;
	int m_Height;

public:
	const char *glslVersion = "#version 150";
	glm::vec4 clearColor = glm::vec4(0.1, 0.1, 0.1, 1.0);
	GLInfo glInfo;
	GLFWwindow *window = NULL;
	// SDL_GLContext glContext = NULL;
	glm::mat4x4 projection;
	glm::mat4x4 view;
	GLContext(std::string title, int width, int height);
	~GLContext();
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }
	void Resize(int width, int height);
};
