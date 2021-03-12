#include "OpenGLRenderbuffer.hpp"
#include "opfor/core/base.hpp"

namespace opfor
{

UniquePtr<Renderbuffer> Renderbuffer::Create()
{
    return MakeUnique<OpenGLRenderbuffer>();
}

OpenGLRenderbuffer::OpenGLRenderbuffer()
{
    glGenRenderbuffers(1, &_RendererID);
}

OpenGLRenderbuffer::~OpenGLRenderbuffer()
{
    glDeleteRenderbuffers(1, &_RendererID);
}

void OpenGLRenderbuffer::Bind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, _RendererID);
}

void OpenGLRenderbuffer::Unbind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void OpenGLRenderbuffer::AttachDepthComponent(std::array<unsigned int, 2> size)
{
    OP4_CORE_ASSERT(size[0] > 0 && size[1] > 0, "Invalid size!\n");

    glBindRenderbuffer(GL_RENDERBUFFER, _RendererID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size[0], size[1]);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void OpenGLRenderbuffer::Resize(unsigned int width, unsigned int height)
{
    glBindRenderbuffer(GL_RENDERBUFFER, _RendererID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

} // namespace opfor
