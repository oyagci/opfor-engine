#pragma once

#include <array>
#include "engine/core/base.hpp"

namespace opfor {

// Forward Declarations
// ====================
class VertexArray;
class Framebuffer;
class Texture;

enum class RendererCaps;
enum class CopyTarget;
enum class TextureUnit;


class RendererAPI
{
public:
	enum class API {
		None = 0,
		OpenGL = 1,
	};

private:
	static API _API;

public:
	virtual ~RendererAPI() = default;

	virtual void Clear() = 0;
	virtual void SetClearColor(std::array<float, 4>) = 0;

	virtual void DrawIndexed(SharedPtr<VertexArray> const &) = 0;

	virtual void PushFramebuffer(SharedPtr<Framebuffer> const &) = 0;
	virtual void PopFramebuffer() = 0;

	virtual void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> const &, CopyTarget) const = 0 ;
	virtual void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> const &dst, SharedPtr<Framebuffer> &src, CopyTarget) const = 0;

	virtual void PushCapability(RendererCaps, bool enable) = 0;
	virtual void PopCapability(RendererCaps) = 0;

	virtual void PushTexture(SharedPtr<Texture> const &, TextureUnit) = 0;
	virtual void PopTexture(TextureUnit) = 0;

	inline static API GetAPI() { return _API; }
};

}
