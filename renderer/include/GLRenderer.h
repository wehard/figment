#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Framebuffer.h"

class IRenderer
{
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
};

class GLRenderer
{
private:
	std::shared_ptr<Camera> m_Camera;
	std::shared_ptr<Shader> m_QuadShader;
	std::shared_ptr<Shader> m_FramebufferShader;
	std::shared_ptr<Shader> m_CircleShader;

	unsigned int m_QuadVBO;
	unsigned int m_QuadVAO;

	uint32_t m_Width;
	uint32_t m_Height;

	Framebuffer *m_Framebuffer;

public:
	GLRenderer(uint32_t width, uint32_t height);
	~GLRenderer();
	void Begin(std::shared_ptr<Camera> camera, glm::vec4 clearColor);
	void End();
	void DrawCircle(glm::mat4 transform, glm::vec4 color, int id);
	void DrawQuad(glm::mat4 transform, glm::vec4 color, int id);
	void DrawTexturedQuad(glm::mat4 transform, uint32_t textureId, Shader &shader);
	void OnResize(uint32_t width, uint32_t height);
	Framebuffer *GetFramebuffer() { return m_Framebuffer; }
};
