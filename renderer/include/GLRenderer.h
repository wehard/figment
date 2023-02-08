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

public:
	GLRenderer();
	~GLRenderer();
	void Begin(OrthoCamera &camera, glm::vec4 clearColor);
	void Draw(GLObject &object);
	void Draw(GLObject &object, Shader &shader);
	void DrawBillboard(GLObject &object, float scale, Shader &shader);
	void DrawLines(GLObject &object, Shader &shader);
};
