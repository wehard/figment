#pragma once

#include "GLObject.h"
#include "Shader.h"
#include "OrthoCamera.h"

class IRenderer
{
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void Submit(GLObject &obj) = 0;
};

class GLRenderer
{
private:
	OrthoCamera *camera;

	unsigned int m_QuadVBO;
	unsigned int m_QuadVAO;

public:
	GLRenderer();
	~GLRenderer();
	void Begin(OrthoCamera &camera, glm::vec4 clearColor);
	void Draw(GLObject &object);
	void Draw(GLObject &object, Shader &shader);
	void DrawCircle(GLObject &object, Shader &shader);
	void DrawBillboard(GLObject &object, float scale, Shader &shader);
	void DrawLines(GLObject &object, Shader &shader);
	void DrawTexturedQuad(glm::mat4 transform, uint32_t textureId, Shader &shader);
};
