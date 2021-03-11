#include "OpenGLBuffer.hpp"

#include "opfor/core/base.hpp"

namespace opfor
{

// OpenGLVertexBuffer
// ==================

OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, size_t size)
{
    glCreateBuffers(1, &_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, _RendererID);

    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &_RendererID);
}

void OpenGLVertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, _RendererID);
}

void OpenGLVertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::SetLayout(BufferLayout layout)
{
    _Layout = layout;
}

BufferLayout const &OpenGLVertexBuffer::GetLayout() const
{
    return _Layout;
}

// OpenGLIndexBuffer
// =================

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, size_t count) : _Count(count)
{
    glCreateBuffers(1, &_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _RendererID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(*indices), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &_RendererID);
}

void OpenGLIndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _RendererID);
}

void OpenGLIndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

size_t OpenGLIndexBuffer::GetCount() const
{
    return _Count;
}

} // namespace opfor
