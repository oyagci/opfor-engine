#include "Renderer.hpp"

#include "opfor/renderer/VertexArray.hpp"
#include "opfor/renderer/Framebuffer.hpp"
#include <glad/glad.h>

#if defined(OP4_PLATFORM_LINUX)
# define __OP4_FUNCNAME__ __PRETTY_FUNCTION__
#elif defined(OP4_PLATFORM_WINDOWS)
# define __OP4_FUNCNAME__ __FUNCTION__
#endif

namespace opfor {

std::list<std::function<void()>> Renderer::_Calls;
std::vector<std::string> _CallsStrings;

void Renderer::PrintTree(unsigned int offset)
{
	fmt::print("=====================================================\n");
	for (auto const &s : _CallsStrings) {
		fmt::print("{}\n", s);
	}
}

template <typename FuncType, typename ... ArgTypes>
auto BIND(FuncType fn, ArgTypes&& ... args)
{
	return std::bind(fn, std::forward<ArgTypes>(args)...);
}

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
	_Calls.clear();
}

void Renderer::EndScene()
{
	// PrintTree();
	_CallsStrings.clear();

	while (!_Calls.empty()) {
		auto nextCall = _Calls.front();

		nextCall();

		_Calls.pop_front();
	}
}

void Renderer::PushViewport(glm::uvec2 pos, glm::uvec2 size)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::PushViewport, pos, size));
}

void Renderer::PopViewport()
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::PopViewport));
}

void Renderer::SetClearColor(std::array<float, 4> const &color)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::SetClearColor, std::forward<decltype(color)>(color)));
}

void Renderer::Clear(ClearFlag flag)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::Clear, flag));
}

void Renderer::Submit(SharedPtr<VertexArray> const &vertexArray)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([vertexArray] () {
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	});
}

void Renderer::Submit(const std::function<void ()> &f)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(f);
}

void Renderer::PushFramebuffer(SharedPtr<Framebuffer> fb)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::PushFramebuffer, fb));
}

void Renderer::PopFramebuffer()
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::PopFramebuffer));
}

void Renderer::PushCapability(RendererCaps cap, bool enable)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::PushCapability, cap, enable));
}

void Renderer::PopCapability(RendererCaps cap)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::PopCapability, cap));
}

void Renderer::CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> dst, SharedPtr<Framebuffer> src, CopyTarget target)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::CopyFramebufferToFramebuffer, dst, src, target));
}

void Renderer::CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> src, CopyTarget target)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(
		BIND(RenderCommand::CopyFramebufferToDefaultFramebuffer, src, target)
	);
}

void Renderer::CopyDefaultFramebufferTo(SharedPtr<Framebuffer> dst, CopyTarget target)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(
		BIND(RenderCommand::CopyDefaultFramebufferTo, dst, target)
	);
}

void Renderer::PushTexture(SharedPtr<Texture> texture, TextureUnit unit)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(
		BIND(RenderCommand::PushTexture, texture, unit)
	);
}

void Renderer::PopTexture(TextureUnit unit)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(
		BIND(RenderCommand::PopTexture, unit)
	);
}

void Renderer::SetDepthMask(bool val)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::SetDepthMask, val));
}

// Shader
// ======

void Renderer::Shader::Push(SharedPtr<opfor::Shader> shader)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::PushShader, shader));
}

void Renderer::Shader::Pop()
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back(BIND(RenderCommand::PopShader));
}

void Renderer::Shader::SetUniform(std::string const &name, size_t value)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([name, value] () { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(std::string const &name, int32_t value)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([name, value] () { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(std::string const &name, uint32_t value)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([name, value] () { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(std::string const &name, float value)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([name, value] () { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(std::string const &name, glm::vec3 value)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([name, value] () { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(std::string const &name, glm::vec4 value)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([name, value] () { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(std::string const &name, glm::mat3 value)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([name, value] () { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(std::string const &name, glm::mat4 value)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([name, value] () { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(std::string const &name, std::vector<glm::mat4> matrices, std::optional<size_t> size)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([name, matrices, size] () { RenderCommand::SetUniform(name, matrices, size); });
}

void Renderer::Shader::SetUniform(std::string const &name, std::vector<glm::vec3> vectors, std::optional<size_t> size)
{
	_CallsStrings.push_back(__OP4_FUNCNAME__);
	_Calls.push_back([name, vectors, size] () { RenderCommand::SetUniform(name, vectors, size); });
}


}
