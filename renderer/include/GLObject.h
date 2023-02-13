#pragma once

#include "GLContext.h"
#include <glm/glm.hpp>
#include <vector>

class GLObject
{
private:
public:
	GLsizei vertexCount;
	int m_Id = 1;
	GLuint vao;
	GLuint vbo;
	GLuint cbo;
	glm::vec3 position = glm::vec3(0.0);
	glm::vec3 rotation = glm::vec3(0.0);
	glm::vec3 scale = glm::vec3(1.0);
	glm::vec4 color = glm::vec4(1.0);
	GLObject(std::vector<float> vertexPositions);
	GLObject(std::vector<float> vertexPositions, std::vector<float> vertexColors);
	~GLObject();
	glm::mat4 getModelMatrix();
	static std::vector<float> Plane();
	static GLObject Triangle();
	static GLObject Axis();
	static std::vector<float> Star();
	static std::vector<float> Grid(int x, int y);
	static std::vector<float> Circle(float radius);
	static std::vector<float> Cube();
};
