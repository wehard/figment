#pragma once

#include <memory>

class VertexArray
{
public:
    static std::shared_ptr<VertexArray> Create();

    virtual ~VertexArray() = default;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;
};
