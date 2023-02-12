#pragma once

#include "GL/glew.h"
#include <vector>

struct FramebufferDesc
{
	GLuint m_Width;
	GLuint m_Height;
};

struct FramebufferTextureDesc
{
	GLint m_InternalFormat;
	GLint m_Format;
};

class Framebuffer
{
public:
	Framebuffer(const FramebufferDesc &desc);
	~Framebuffer();
	void Recreate();
	void Bind();
	void Unbind();
	void ClearAttachment(GLuint index, int value);

private:
	FramebufferDesc m_Desc;
	GLuint m_FboID;
	GLuint rbo_id;

	std::vector<FramebufferTextureDesc> m_ColorAttachmentDescs;
	FramebufferTextureDesc m_DepthAttachmentDesc;

	std::vector<GLuint> m_ColorAttachments;
	GLuint m_DepthAttachment;
};
