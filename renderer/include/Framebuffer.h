#pragma once

#include "GL/glew.h"
#include <vector>
#include <glm/vec4.hpp>

struct FramebufferDesc
{
	GLuint m_Width;
	GLuint m_Height;
};

struct FramebufferTextureDesc
{
	GLint m_InternalFormat;
	GLenum m_Format;
	GLenum m_Type;
};

class Framebuffer
{
public:
	enum class TextureFormat
	{
		RGBA8 = GL_RGBA8,
		RED_INTEGER = GL_RED_INTEGER,
		Depth = GL_DEPTH24_STENCIL8

	};

	Framebuffer(const FramebufferDesc &desc);
	~Framebuffer();
	void Recreate();
	void Bind();
	void Unbind();
	void ClearAttachment(GLuint index, int value);
	void Resize(uint32_t width, uint32_t height);
	GLuint GetColorAttachmentId(uint32_t index = 0) const
	{
		return m_ColorAttachments[index];
	}
	int GetPixel(uint32_t attachmentIndex, int x, int y);

private:
	FramebufferDesc m_Desc;
	GLuint m_FboID;
	GLuint rbo_id;

	std::vector<FramebufferTextureDesc> m_ColorAttachmentDescs;
	FramebufferTextureDesc m_DepthAttachmentDesc;

	std::vector<GLuint> m_ColorAttachments;
	GLuint m_DepthAttachment;
};
