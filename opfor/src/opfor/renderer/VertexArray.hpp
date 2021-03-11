#pragma once

#include "opfor/renderer/Buffer.hpp"

namespace opfor
{

class VertexArray
{
  public:
    virtual ~VertexArray()
    {
    }

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void AddVertexBuffer(SharedPtr<VertexBuffer>) = 0;
    virtual void SetIndexBuffer(SharedPtr<IndexBuffer>) = 0;

    virtual IndexBuffer const *GetIndexBuffer() const = 0;

    static UniquePtr<VertexArray> Create();
};

} // namespace opfor
