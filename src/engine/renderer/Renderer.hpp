#pragma once

#include "RenderCommand.hpp"
#include "lazy.hpp"

namespace opfor {

enum class ClearFlags
{
	ColorBit,
	DepthBit,
};

enum class RendererCaps
{
	DepthTest = GL_DEPTH_TEST,
	CullFace = GL_CULL_FACE,
	Blend = GL_BLEND,
};

enum class RendererCaps;

class Renderer
{
public:
	static void BeginScene();
	static void EndScene();

	static void SetClearColor(std::array<float, 4> const &color);
	static void Clear();

	static void Submit(SharedPtr<VertexArray> const &);

	static void PushFramebuffer(SharedPtr<Framebuffer>);
	static void PopFramebuffer();

	static void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> dst, SharedPtr<Framebuffer> src, CopyTarget);
	static void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> src, CopyTarget);

	static void PushCapability(RendererCaps, bool);
	static void PopCapability(RendererCaps);

	static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};

}
