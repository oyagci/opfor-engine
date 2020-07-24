#include "Renderer.hpp"

#include "engine/renderer/VertexArray.hpp"
#include "engine/renderer/Framebuffer.hpp"

namespace opfor {

ClearFlag operator|(ClearFlag lhs, ClearFlag rhs) {
	return static_cast<ClearFlag>(
			static_cast<unsigned int>(lhs) |
			static_cast<unsigned int>(rhs));
}

ClearFlag operator&(ClearFlag lhs, ClearFlag rhs) {
	return static_cast<ClearFlag>(
			static_cast<unsigned int>(lhs) &
			static_cast<unsigned int>(rhs));
}

void Renderer::BeginScene()
{
}

void Renderer::EndScene()
{
}

void Renderer::PushViewport(glm::uvec2 pos, glm::uvec2 size)
{
	RenderCommand::PushViewport(pos, size);
}

void Renderer::PopViewport()
{
	RenderCommand::PopViewport();
}

void Renderer::SetClearColor(std::array<float, 4> const &color)
{
	RenderCommand::SetClearColor(std::forward<decltype(color)>(color));
}

void Renderer::Clear(ClearFlag flag)
{
	RenderCommand::Clear(flag);
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

// Shader
// ======

void Renderer::Shader::Push(SharedPtr<opfor::Shader> shader)
{
	RenderCommand::PushShader(shader);
}

void Renderer::Shader::Pop()
{
	RenderCommand::PopShader();
}

void Renderer::Shader::SetUniform(std::string const &name, size_t value)
{
	RenderCommand::SetUniform(name, value);
}

void Renderer::Shader::SetUniform(std::string const &name, int32_t value)
{
	RenderCommand::SetUniform(name, value);
}

void Renderer::Shader::SetUniform(std::string const &name, uint32_t value)
{
	RenderCommand::SetUniform(name, value);
}

void Renderer::Shader::SetUniform(std::string const &name, float value)
{
	RenderCommand::SetUniform(name, value);
}

void Renderer::Shader::SetUniform(std::string const &name, glm::vec3 value)
{
	RenderCommand::SetUniform(name, value);
}

void Renderer::Shader::SetUniform(std::string const &name, glm::vec4 value)
{
	RenderCommand::SetUniform(name, value);
}

void Renderer::Shader::SetUniform(std::string const &name, glm::mat3 value)
{
	RenderCommand::SetUniform(name, value);
}

void Renderer::Shader::SetUniform(std::string const &name, glm::mat4 value)
{
	RenderCommand::SetUniform(name, value);
}

void Renderer::Shader::SetUniform(std::string const &name, std::vector<glm::mat4> matrices, std::optional<size_t> size)
{
	RenderCommand::SetUniform(name, matrices, size);
}

void Renderer::Shader::SetUniform(std::string const &name, std::vector<glm::vec3> vectors, std::optional<size_t> size)
{
	RenderCommand::SetUniform(name, vectors, size);
}


}
