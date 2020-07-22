#include "Renderer.hpp"

#include "engine/renderer/VertexArray.hpp"
#include "engine/renderer/Framebuffer.hpp"

namespace opfor {

void Renderer::BeginScene()
{
}

void Renderer::EndScene()
{
}

void Renderer::SetClearColor(std::array<float, 4> const &color)
{
	RenderCommand::SetClearColor(std::forward<decltype(color)>(color));
}

void Renderer::Clear()
{
	RenderCommand::Clear();
}

void Renderer::Submit(SharedPtr<VertexArray> const &vertexArray)
{
	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
	vertexArray->Unbind();
}

void Renderer::PushFramebuffer(SharedPtr<Framebuffer> fb)
{
	RenderCommand::PushFramebuffer(fb);
}

void Renderer::PopFramebuffer()
{
	RenderCommand::PopFramebuffer();
}

void Renderer::PushCapability(RendererCaps cap, bool enable)
{
	RenderCommand::PushCapability(cap, enable);
}

void Renderer::PopCapability(RendererCaps cap)
{
	RenderCommand::PopCapability(cap);
}

void Renderer::CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> dst, SharedPtr<Framebuffer> src, CopyTarget target)
{
	RenderCommand::CopyFramebufferToFramebuffer(dst, src, target);
}

void Renderer::CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> src, CopyTarget target)
{
	RenderCommand::CopyFramebufferToDefaultFramebuffer(src, target);
}

void Renderer::PushTexture(SharedPtr<Texture> texture, TextureUnit unit)
{
	RenderCommand::PushTexture(texture, unit);
}

void Renderer::PopTexture(TextureUnit unit)
{
	RenderCommand::PopTexture(unit);
}

}
