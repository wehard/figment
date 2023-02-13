#include "GLRenderer.h"
#include <glm/gtc/matrix_transform.hpp>

GLRenderer::GLRenderer()
{
	float quadVertices[] = {
		-1.0f, -1.0f, 0.0, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0, 0.0f, 0.0f};
	glGenBuffers(1, &m_QuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &m_QuadVAO);
	glBindVertexArray(m_QuadVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

GLRenderer::~GLRenderer()
{
}

void GLRenderer::Begin(OrthoCamera &camera, glm::vec4 clearColor)
{
	this->camera = &camera;
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GLRenderer::Draw(GLObject &object)
{
	glBindVertexArray(object.vao);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glDrawArrays(GL_TRIANGLES, 0, object.vertexCount);
}

void GLRenderer::Draw(GLObject &object, Shader &shader)
{
	shader.use();
	shader.setVec4("obj_color", object.color);
	shader.setMat4("proj_matrix", camera->GetProjectionMatrix());
	shader.setMat4("view_matrix", camera->GetViewMatrix());
	shader.setMat4("model_matrix", object.getModelMatrix());
	shader.setInt("obj_id", object.m_Id);
	Draw(object);
}

static glm::mat4 getModelMatrixBillboard(GLObject &object, glm::vec3 target)
{
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), object.scale);
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), object.position);
	glm::mat4 matRotate = glm::lookAt(object.position, target, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 m = matTranslate * matRotate * matScale;
	return (m);
}

void GLRenderer::DrawBillboard(GLObject &object, float scale, Shader &shader)
{
	shader.use();
	shader.setVec4("obj_color", object.color);
	shader.setMat4("proj_matrix", camera->GetProjectionMatrix());
	shader.setMat4("view_matrix", camera->GetViewMatrix());
	shader.setMat4("model_matrix", object.getModelMatrix());
	shader.setFloat("b_scale", scale);
	Draw(object);
}

void GLRenderer::DrawLines(GLObject &object, Shader &shader)
{
	shader.use();
	shader.setVec4("obj_color", object.color);
	shader.setMat4("proj_matrix", camera->GetProjectionMatrix());
	shader.setMat4("view_matrix", camera->GetViewMatrix());
	shader.setMat4("model_matrix", object.getModelMatrix());

	glBindVertexArray(object.vao);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glDrawArrays(GL_LINES, 0, object.vertexCount);
}

void GLRenderer::DrawTexturedQuad(glm::mat4 transform, uint32_t textureId, Shader &shader)
{
	shader.use();
	shader.setMat4("model_matrix", transform);
	glBindVertexArray(m_QuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
