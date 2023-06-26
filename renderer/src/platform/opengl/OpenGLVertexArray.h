#pragma once
#include "VertexArray.h"
#include <stdint.h>

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray();
    void Bind();
    void Unbind();

private:
    uint32_t m_Id;
};
