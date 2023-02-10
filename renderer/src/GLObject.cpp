#include "GLObject.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <vector>

GLObject::GLObject(std::vector<float> vertexPositions) : vertexCount(vertexPositions.size() / 3)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexPositions.size(), &vertexPositions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLObject::GLObject(std::vector<float> vertexPositions, std::vector<float> vertexColors) : vertexCount(vertexPositions.size() / 3)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &cbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexPositions.size(), &vertexPositions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexColors.size(), &vertexColors[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLObject::~GLObject()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteVertexArrays(1, &vao);
}

glm::mat4 GLObject::getModelMatrix()
{
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
	glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
	glm::mat4 m = matTranslate * matRotate * matScale;
	return (m);
}

std::vector<float> GLObject::Plane()
{
	return std::vector<float>{
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f};
}

GLObject GLObject::Triangle()
{
	auto triangle = GLObject(std::vector<float>{
		-0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f});
	triangle.color = glm::vec4(1.0, 1.0, 1.0, 1.0);
	return triangle;
}

std::vector<float> GLObject::Star()
{
	auto star = std::vector<float>{
		-0.5f, -0.25f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, -0.25f, 0.0f,
		0.0f, -0.5f, 0.0f,
		-0.5f, 0.25f, 0.0f,
		0.5f, 0.25f, 0.0f};
	return star;
}

GLObject GLObject::Axis()
{
	auto axis = GLObject(std::vector<float>{
							 0.0, 0.0, 0.0,
							 1.0, 0.0, 0.0,
							 0.0, 0.0, 0.0,
							 0.0, 1.0, 0.0,
							 0.0, 0.0, 0.0,
							 0.0, 0.0, 1.0},
						 std::vector<float>{1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0});
	axis.color = glm::vec4(1.0, 1.0, 1.0, 1.0);
	return axis;
}

std::vector<float> GLObject::Grid(int xs, int ys)
{
	std::vector<float> vertices;

	float xi = 1.0 / (float)xs;
	float yi = 1.0 / (float)ys;

	for (size_t x = 0; x <= xs; x++)
	{
		float cx = x * xi;
		vertices.push_back(cx - 0.5);
		vertices.push_back(-0.5);
		vertices.push_back(0.0);

		vertices.push_back(cx - 0.5);
		vertices.push_back(0.5);
		vertices.push_back(0.0);
	}
	for (size_t y = 0; y <= ys; y++)
	{
		float cy = y * yi;
		vertices.push_back(-0.5);
		vertices.push_back(cy - 0.5);
		vertices.push_back(0.0);

		vertices.push_back(0.5);
		vertices.push_back(cy - 0.5);
		vertices.push_back(0.0);
	}

	return vertices;
}

std::vector<float> GLObject::Circle(float radius)
{
	std::vector<float> vertices;

	int steps = 64;

	glm::vec2 p = glm::vec2(0.0, 1.0);

	for (int i = 0; i < steps + 1; i++)
	{
		vertices.push_back(0.0);
		vertices.push_back(0.0);
		vertices.push_back(0.0);

		vertices.push_back(p.x);
		vertices.push_back(p.y);
		vertices.push_back(0.0);

		float a = ((M_PI * 2.0) / (float)steps) * i;

		glm::vec2 next = glm::vec2(0.0 * cos(a) - 1.0 * sin(a), 1.0 * cos(a) + 0.0 * sin(a));

		vertices.push_back(next.x);
		vertices.push_back(next.y);
		vertices.push_back(0.0);

		p = next;
	}
	return vertices;
}

std::vector<float> GLObject::Cube()
{
	return std::vector<float>{
		-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5,
		-0.5, -0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5,
		0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5,
		0.5, -0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5,
		0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5,
		0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5,
		-0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5,
		-0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5,
		-0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5,
		-0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5,
		0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5,
		0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5};
}
