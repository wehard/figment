#pragma once

#include "Shader.h"
#include "OrthoCamera.h"
#include "Framebuffer.h"

class IRenderer
{
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	// virtual void Submit(GLObject &obj) = 0;
};

class GLRenderer
{
private:
	OrthoCamera *m_Camera;
	Shader *m_QuadShader;
	Shader *m_FramebufferShader;
	Shader *m_CircleShader;

	unsigned int m_QuadVBO;
	unsigned int m_QuadVAO;

	uint32_t m_Width;
	uint32_t m_Height;

public:
	Framebuffer *m_Framebuffer;
	GLRenderer(uint32_t width, uint32_t height);
	~GLRenderer();
	void Begin(OrthoCamera &camera, glm::vec4 clearColor);
	void End();
	void DrawCircle(glm::mat4 transform, glm::vec4 color, int id);
	void DrawQuad(glm::mat4 transform, glm::vec4 color, int id);
	void DrawTexturedQuad(glm::mat4 transform, uint32_t textureId, Shader &shader);
	void OnResize(uint32_t width, uint32_t height);
};
