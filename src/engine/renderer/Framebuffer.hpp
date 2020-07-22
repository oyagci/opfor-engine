#pragma once

#include "lazy.hpp"
#include "engine/core/base.hpp"

namespace opfor {

class Texture;
class Renderbuffer;

enum class FramebufferAttachment
{
#ifdef OP4_PLATFORM_OPENGL
	ColorAttachment0  = GL_COLOR_ATTACHMENT0,
	ColorAttachment1  = GL_COLOR_ATTACHMENT1,
	ColorAttachment2  = GL_COLOR_ATTACHMENT2,
	ColorAttachment3  = GL_COLOR_ATTACHMENT3,
	ColorAttachment4  = GL_COLOR_ATTACHMENT4,
	ColorAttachment5  = GL_COLOR_ATTACHMENT5,
	ColorAttachment6  = GL_COLOR_ATTACHMENT6,
	ColorAttachment7  = GL_COLOR_ATTACHMENT7,
	ColorAttachment8  = GL_COLOR_ATTACHMENT8,
	ColorAttachment9  = GL_COLOR_ATTACHMENT9,
	ColorAttachment10 = GL_COLOR_ATTACHMENT10,
	ColorAttachment11 = GL_COLOR_ATTACHMENT11,
	ColorAttachment12 = GL_COLOR_ATTACHMENT12,
	ColorAttachment13 = GL_COLOR_ATTACHMENT13,
	ColorAttachment14 = GL_COLOR_ATTACHMENT14,
	ColorAttachment15 = GL_COLOR_ATTACHMENT15,
#endif
};

enum class RenderbufferAttachment
{
#ifdef OP4_PLATFORM_OPENGL
	DepthAttachment = GL_DEPTH_ATTACHMENT,
	DepthStencilAttachment = GL_DEPTH_STENCIL_ATTACHMENT,
#endif
};

class Framebuffer
{
private:
public:
	virtual ~Framebuffer() {}

	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual void AttachTexture(SharedPtr<Texture>, FramebufferAttachment) = 0;
	virtual void AttachRenderbuffer(SharedPtr<Renderbuffer>, RenderbufferAttachment) = 0;

	virtual uint32_t GetRawHandle() const = 0;

	virtual bool IsComplete() const = 0;

	static UniquePtr<Framebuffer> Create();
};

}
