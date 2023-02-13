#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class OpenGLVertexBuffer
{
private:
    uint32_t m_Id;

public:
    OpenGLVertexBuffer(float *vertices, uint32_t size);
    ~OpenGLVertexBuffer();
    void Bind();
    void Unbind();
};

OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, uint32_t size)
{
    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &m_Id);
}

void OpenGLVertexBuffer::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void OpenGLVertexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

class OpenGLIndexBuffer
{
private:
    uint32_t m_Id;
    uint32_t m_Size;

public:
    OpenGLIndexBuffer(uint32_t *indices, uint32_t size);
    ~OpenGLIndexBuffer();
};

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t size) : m_Size(size)
{
    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &m_Id);
}
