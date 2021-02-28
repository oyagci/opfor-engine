#pragma once

#include "renderer.hpp"
#include "opfor/core/base.hpp"
#include "opfor/core/base.hpp"

namespace opfor {

class Texture;
class Renderbuffer;

enum class FramebufferAttachment
{
	ColorAttachment0  = 0,
	ColorAttachment1  = 1,
	ColorAttachment2  = 2,
	ColorAttachment3  = 3,
	ColorAttachment4  = 4,
	ColorAttachment5  = 5,
	ColorAttachment6  = 6,
	ColorAttachment7  = 7,
	ColorAttachment8  = 8,
	ColorAttachment9  = 9,
	ColorAttachment10 = 10,
	ColorAttachment11 = 11,
	ColorAttachment12 = 12,
	ColorAttachment13 = 13,
	ColorAttachment14 = 14,
	ColorAttachment15 = 15,
	DepthAttachment   = 16,
};

enum class RenderbufferAttachment
{
	DepthAttachment = 1,
	DepthStencilAttachment = 2,
};

enum class CopyTarget
{
	ColorBufferBit   = BIT(0),
	DepthBufferBit   = BIT(1),
	StencilBufferBit = BIT(2),
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

	virtual void CopyToDefault(CopyTarget target) = 0;
	virtual void CopyTo(CopyTarget target, Framebuffer &dst) = 0;
	virtual void CopyDefault(CopyTarget target) = 0;

	static UniquePtr<Framebuffer> Create();
};

}
