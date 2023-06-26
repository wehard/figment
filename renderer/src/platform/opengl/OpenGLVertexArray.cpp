#include "OpenGLVertexArray.h"
#include <GL/glew.h>

OpenGLVertexArray::OpenGLVertexArray()
{
    glGenVertexArrays(1, &m_Id);
    glBindVertexArray(m_Id);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
}

void OpenGLVertexArray::Bind()
{
}

void OpenGLVertexArray::Unbind()
{
}
