#pragma once

#include "Buffer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

class OpenGLVertexBuffer : public VertexBuffer
{
private:
    uint32_t m_Id;

public:
    OpenGLVertexBuffer(std::vector<float> vertices);
    ~OpenGLVertexBuffer();
    void Bind();
    void Unbind();
};

OpenGLVertexBuffer::OpenGLVertexBuffer(std::vector<float> vertices)
{
    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
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

class OpenGLIndexBuffer : public IndexBuffer
{
private:
    uint32_t m_Id;
    void Bind();
    void Unbind();

public:
    OpenGLIndexBuffer(std::vector<uint32_t> indices);
    ~OpenGLIndexBuffer();
};

OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<uint32_t> indices)
{
    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
    glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &m_Id);
}

void OpenGLIndexBuffer::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void OpenGLIndexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
