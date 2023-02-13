#include "Framebuffer.h"
#include "glm/vec4.hpp"
#include <iostream>

static GLuint CreateTexture(int32_t width, int32_t height, GLint internalFormat, GLenum format)
{
	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

static void SetTextureData(GLuint textureId, int32_t width, int32_t height, void *data)
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

Framebuffer::Framebuffer(const FramebufferDesc &desc) : m_Desc(desc)
{
	FramebufferTextureDesc t;
	t.m_InternalFormat = GL_RGBA;
	t.m_Format = GL_RGBA;
	m_ColorAttachmentDescs.emplace_back(t);

	FramebufferTextureDesc id;
	id.m_InternalFormat = GL_R8UI;
	id.m_Format = GL_RED_INTEGER;
	m_ColorAttachmentDescs.emplace_back(id);

	m_DepthAttachmentDesc.m_InternalFormat = GL_DEPTH_COMPONENT24;
	m_DepthAttachmentDesc.m_Format = GL_DEPTH_COMPONENT;
	Recreate();
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_FboID);
}

void Framebuffer::Recreate()
{
	if (m_FboID)
	{
		glDeleteFramebuffers(1, &m_FboID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	glGenFramebuffers(1, &m_FboID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);

	m_ColorAttachments.resize(m_ColorAttachmentDescs.size());

	for (size_t i = 0; i < m_ColorAttachments.size(); i++)
	{
		m_ColorAttachments[i] = CreateTexture(m_Desc.m_Width, m_Desc.m_Height, m_ColorAttachmentDescs[i].m_InternalFormat, m_ColorAttachmentDescs[i].m_Format);
	}

	for (size_t i = 0; i < m_ColorAttachments.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, m_ColorAttachmentDescs[i].m_InternalFormat, m_Desc.m_Width, m_Desc.m_Height, 0, m_ColorAttachmentDescs[i].m_Format, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i], 0);
	}

	{
		glGenTextures(1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

		glTexImage2D(GL_TEXTURE_2D, 0, m_DepthAttachmentDesc.m_InternalFormat, m_Desc.m_Width, m_Desc.m_Height, 0, m_DepthAttachmentDesc.m_Format, GL_UNSIGNED_INT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
	}

	if (m_ColorAttachments.size() > 1)
	{
		GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
		glDrawBuffers(m_ColorAttachments.size(), buffers);
	}
	else if (m_ColorAttachments.empty())
	{
		// Only depth-pass
		GLenum buffers[1] = {GL_NONE};
		glDrawBuffers(1, buffers);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	else
		std::cout << "Framebuffer created." << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(uint32_t width, uint32_t height)
{
	m_Desc.m_Width = width;
	m_Desc.m_Height = height;
	Recreate();
}

void Framebuffer::ClearAttachment(GLuint index, int value)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);
	static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, draw_buffers);

	static const float c[] = {0.15, 0.15, 0.15, 1.0};
	glClearBufferfv(GL_COLOR, 0, c);
	glClearBufferuiv(GL_COLOR, 1, 0);
}

GLuint Framebuffer::GetPixel(uint32_t attachmentIndex, int x, int y)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
	GLuint pixel = 0;
	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &pixel);
	return pixel;
}
