#pragma once

#include "RenderCommand.hpp"
#include "lazy.hpp"
#include <list>

namespace opfor {

enum class ClearFlag
{
	None     = 0,
	ColorBit = 1 << 0,
	DepthBit = 1 << 1,

};

ClearFlag operator|(ClearFlag lhs, ClearFlag rhs);
ClearFlag operator&(ClearFlag lhs, ClearFlag rhs);

enum class RendererCaps
{
#ifdef OP4_PLATFORM_OPENGL
	DepthTest = GL_DEPTH_TEST,
	CullFace = GL_CULL_FACE,
	Blend = GL_BLEND,
#endif
};

enum class RendererCaps;

class Renderer
{
private:
	static std::list<std::function<void()>> _Calls;

	static void PrintTree(unsigned int offset = 0);

public:
	static void BeginScene();
	static void EndScene();

	static void SetDepthMask(bool);

	static void PushViewport(glm::uvec2 pos, glm::uvec2 size);
	static void PopViewport();

	static void SetClearColor(std::array<float, 4> const &color);
	static void Clear(ClearFlag);

	static void Submit(SharedPtr<VertexArray> const &);
	static void Submit(std::function<void()> const &);

	static void PushFramebuffer(SharedPtr<Framebuffer>);
	static void PopFramebuffer();

	static void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> dst, SharedPtr<Framebuffer> src, CopyTarget);
	static void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> src, CopyTarget);
	static void CopyDefaultFramebufferTo(SharedPtr<Framebuffer> dst, CopyTarget);

	static void PushCapability(RendererCaps, bool);
	static void PopCapability(RendererCaps);

	static void PushTexture(SharedPtr<Texture>, TextureUnit);
	static void PopTexture(TextureUnit);

	static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	class Shader
	{
	public:
		static void Push(SharedPtr<opfor::Shader>);
		static void Pop();

		static void SetUniform(std::string const &name, size_t value);
		static void SetUniform(std::string const &name, int32_t value);
		static void SetUniform(std::string const &name, uint32_t value);
		static void SetUniform(std::string const &name, float value);
		static void SetUniform(std::string const &name, glm::vec3 value);
		static void SetUniform(std::string const &name, glm::vec4 value);
		static void SetUniform(std::string const &name, glm::mat3 value);
		static void SetUniform(std::string const &name, glm::mat4 value);
		static void SetUniform(std::string const &name, std::vector<glm::mat4> matrices, std::optional<size_t> size = std::nullopt);
		static void SetUniform(std::string const &name, std::vector<glm::vec3> vectors, std::optional<size_t> size = std::nullopt);
	};
};

}
