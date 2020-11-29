#pragma once

#include <glad/glad.h>
#include "RenderCommand.hpp"
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
	CullFace  = GL_CULL_FACE,
	Blend     = GL_BLEND,
#endif
};

struct DrawCommandTextureBinding
{
	SharedPtr<Texture> texture;
	TextureUnit binding;
};

struct UniformBinding
{
	using BindingValue = Variant<int, float, glm::vec3, glm::vec4, glm::mat4, Vector<glm::mat4>>;
	
	String name;
	BindingValue value;
};

struct ClearScreenParam
{
	std::array<float, 4> color;
	ClearFlag flags;
};

struct DrawCommand
{
	SharedPtr<Shader> shader;
	SharedPtr<VertexArray> vertexArray;
	Vector<DrawCommandTextureBinding> textureBindings;
	Vector<UniformBinding> uniformBindings;
};

struct RenderCommandBuffer
{
	Optional<SharedPtr<Framebuffer>> framebuffer;
	Optional<std::pair<glm::uvec2, glm::uvec2>> viewportExtent;
	Optional<ClearScreenParam> clear;
	Vector<std::pair<RendererCaps, bool>> capabilities;
	Vector<DrawCommand> drawCommands;
	bool disableDepthMask;
};

struct RenderCommandBufferOptimized
{
	Optional<SharedPtr<Framebuffer>> framebuffer;
	Optional<std::pair<glm::uvec2, glm::uvec2>> viewportExtent;
	Optional<ClearScreenParam> clear;
	Vector<std::pair<RendererCaps, bool>> capabilities;
	std::unordered_map<SharedPtr<opfor::Shader>, Vector<DrawCommand>> drawCommandsByShader;
	bool disableDepthMask;

};

enum class RendererCaps;

class Renderer
{
private:
	static std::list<std::function<void()>> _Calls;

	static inline std::list<RenderCommandBufferOptimized> _OptimizedRenderCommands;

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

	static void SubmitDrawCommand(DrawCommand const &drawCommand);
	static void SubmitRenderCommandBuffer(RenderCommandBuffer const &renderCommand);

	static void ExecRenderCommandBuffer(RenderCommandBufferOptimized const &renderCommand);

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
