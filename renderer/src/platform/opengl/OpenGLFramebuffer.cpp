#include "OpenGLFramebuffer.h"
#include <glm/vec4.hpp>
#include <iostream>

static GLuint CreateTexture(int32_t width, int32_t height, GLint internalFormat, GLenum format, GLenum type)
{
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

static void SetTextureData(GLuint textureId, int32_t width, int32_t height, void *data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDesc &desc)
{
    FramebufferTextureDesc t;
    t.m_InternalFormat = GL_RGBA;
    t.m_Format = GL_RGBA;
    t.m_Type = GL_UNSIGNED_BYTE;
    m_ColorAttachmentDescs.emplace_back(t);

    FramebufferTextureDesc id;
    id.m_InternalFormat = GL_R32I;
    id.m_Format = GL_RED_INTEGER;
    id.m_Type = GL_INT;
    m_ColorAttachmentDescs.emplace_back(id);

    m_DepthAttachmentDesc.m_InternalFormat = GL_DEPTH_COMPONENT24;
    m_DepthAttachmentDesc.m_Format = GL_DEPTH_COMPONENT;
    m_DepthAttachmentDesc.m_Type = GL_UNSIGNED_INT;
    Recreate();
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    glDeleteFramebuffers(1, &m_FboID);
}

void OpenGLFramebuffer::Recreate()
{
    if (m_FboID)
    {
        glDeleteFramebuffers(1, &m_FboID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);
    }

    glGenFramebuffers(1, &m_FboID);

    Bind();

    m_ColorAttachments.resize(m_ColorAttachmentDescs.size());

    for (size_t i = 0; i < m_ColorAttachments.size(); i++)
    {
        m_ColorAttachments[i] = CreateTexture(m_Desc.m_Width, m_Desc.m_Height, m_ColorAttachmentDescs[i].m_InternalFormat, m_ColorAttachmentDescs[i].m_Format, m_ColorAttachmentDescs[i].m_Type);
    }

    for (size_t i = 0; i < m_ColorAttachments.size(); i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, m_ColorAttachmentDescs[i].m_InternalFormat, m_Desc.m_Width, m_Desc.m_Height, 0, m_ColorAttachmentDescs[i].m_Format, m_ColorAttachmentDescs[i].m_Type, NULL);
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

        glTexImage2D(GL_TEXTURE_2D, 0, m_DepthAttachmentDesc.m_InternalFormat, m_Desc.m_Width, m_Desc.m_Height, 0, m_DepthAttachmentDesc.m_Format, m_DepthAttachmentDesc.m_Type, NULL);

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
        std::cout << "OpenGL framebuffer created: " << m_Desc.m_Width << " x " << m_Desc.m_Height << std::endl;

    Unbind();
}

void OpenGLFramebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);
}

void OpenGLFramebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::ClearAttachment(GLuint index, glm::vec4 clearColor, int clearId)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);
    static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, draw_buffers);

    float c[] = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
    glClearBufferfv(GL_COLOR, 0, c);
    glClearBufferiv(GL_COLOR, 1, &clearId);
}

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
{
    m_Desc.m_Width = width;
    m_Desc.m_Height = height;
    Recreate();
}

int OpenGLFramebuffer::GetPixel(uint32_t attachmentIndex, int x, int y)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    int pixel = 0;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
    return pixel;
}
