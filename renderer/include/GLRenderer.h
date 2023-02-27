#pragma once

#include "GLObject.h"
#include "Shader.h"
#include "OrthoCamera.h"
#include "Framebuffer.h"

class IRenderer
{
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void Submit(GLObject &obj) = 0;
};

class GLRenderer
{
private:
	OrthoCamera *m_Camera;
	Shader *m_QuadShader;
	Shader *m_FramebufferShader;

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
	void Draw(GLObject &object);
	void Draw(GLObject &object, Shader &shader);
	void DrawCircle(GLObject &object, Shader &shader);
	void DrawQuad(glm::mat4 transform, int id);
	void DrawBillboard(GLObject &object, float scale, Shader &shader);
	void DrawLines(GLObject &object, Shader &shader);
	void DrawTexturedQuad(glm::mat4 transform, uint32_t textureId, Shader &shader);
	void OnResize(uint32_t width, uint32_t height);
};
