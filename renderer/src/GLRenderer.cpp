#include "GLRenderer.h"
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <sstream>

static std::string readFile(std::string path)
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

GLRenderer::GLRenderer(uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
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

	FramebufferDesc desc;
	desc.m_Width = width;
	desc.m_Height = height;
	m_Framebuffer = new Framebuffer(desc);

	m_QuadShader = new Shader(readFile("shaders/basic.vert").c_str(), readFile("shaders/basic.frag").c_str());
	m_CircleShader = new Shader(readFile("shaders/circle.vert").c_str(), readFile("shaders/circle.frag").c_str());
	m_FramebufferShader = new Shader(readFile("shaders/framebuffer.vert").c_str(), readFile("shaders/framebuffer.frag").c_str());
}

GLRenderer::~GLRenderer()
{
	delete m_Framebuffer;
	delete m_QuadShader;
	delete m_FramebufferShader;
}

void GLRenderer::Begin(OrthoCamera &camera, glm::vec4 clearColor)
{
	m_Camera = &camera;
	m_Framebuffer->Bind();
	m_Framebuffer->ClearAttachment(0, -1);
}

void GLRenderer::End()
{
	m_Framebuffer->Unbind();
	DrawTexturedQuad(glm::identity<glm::mat4>(), m_Framebuffer->GetColorAttachmentId(0), *m_FramebufferShader);
}

void GLRenderer::DrawCircle(glm::mat4 transform, glm::vec4 color, int id)
{
	m_CircleShader->use();
	m_CircleShader->setVec4("obj_color", color);
	m_CircleShader->setMat4("proj_matrix", m_Camera->GetProjectionMatrix());
	m_CircleShader->setMat4("view_matrix", m_Camera->GetViewMatrix());
	m_CircleShader->setMat4("model_matrix", transform);
	m_CircleShader->setInt("obj_id", id);

	glBindVertexArray(m_QuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GLRenderer::DrawQuad(glm::mat4 transform, glm::vec4 color, int id)
{
	m_QuadShader->use();
	m_QuadShader->setVec4("obj_color", color);
	m_QuadShader->setMat4("proj_matrix", m_Camera->GetProjectionMatrix());
	m_QuadShader->setMat4("view_matrix", m_Camera->GetViewMatrix());
	m_QuadShader->setMat4("model_matrix", transform);
	m_QuadShader->setInt("obj_id", id);

	glBindVertexArray(m_QuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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

void GLRenderer::OnResize(uint32_t width, uint32_t height)
{
	m_Framebuffer->Resize(width, height);
}
