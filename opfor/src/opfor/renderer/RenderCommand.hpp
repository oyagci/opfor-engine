#pragma once

#include "RendererAPI.hpp"

namespace opfor {

class RenderCommand
{
	private:
		static UniquePtr<RendererAPI> _API;

	public:
		static inline void PushViewport(glm::uvec2 pos, glm::uvec2 size)
		{
			_API->PushViewport(pos, size);
		}

		static inline void PopViewport()
		{
			_API->PopViewport();
		}

		static inline void SetClearColor(std::array<float, 4> const color)
		{
			_API->SetClearColor(std::forward<decltype(color)>(color));
		}

		static inline void Clear(ClearFlag flag)
		{
			_API->Clear(flag);
		}

		static inline void DrawIndexed(SharedPtr<VertexArray> const &vertexArray)
		{
			_API->DrawIndexed(std::forward<decltype(vertexArray)>(vertexArray));
		}

		static inline void PushFramebuffer(SharedPtr<Framebuffer> const &fb)
		{
			_API->PushFramebuffer(fb);
		}

		static inline void PopFramebuffer()
		{
			_API->PopFramebuffer();
		}

		static inline void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> fb, CopyTarget target)
		{
			_API->CopyFramebufferToDefaultFramebuffer(fb, target);
		}

		static inline void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> dst, SharedPtr<Framebuffer> src, CopyTarget target)
		{
			_API->CopyFramebufferToFramebuffer(dst, src, target);
		}

		static inline void CopyDefaultFramebufferTo(SharedPtr<Framebuffer> dst, CopyTarget target)
		{
			_API->CopyDefaultFramebufferTo(dst, target);
		}

		static inline void PushCapability(RendererCaps cap, bool enable)
		{
			_API->PushCapability(cap, enable);
		}

		static inline void PopCapability(RendererCaps cap)
		{
			_API->PopCapability(cap);
		}

		static inline void PushTexture(SharedPtr<Texture> texture, TextureUnit unit)
		{
			_API->PushTexture(texture, unit);
		}

		static inline void PopTexture(TextureUnit unit)
		{
			_API->PopTexture(unit);
		}

		static inline void PushShader(SharedPtr<Shader> shader)
		{
			_API->PushShader(shader);
		}

		static inline void PopShader()
		{
			_API->PopShader();
		}

		static inline void SetUniform(std::string const &name, size_t value)
		{
			_API->SetUniform(name, value);
		}

		static inline void SetUniform(std::string const &name, int32_t value)
		{
			_API->SetUniform(name, value);
		}

		static inline void SetUniform(std::string const &name, uint32_t value)
		{
			_API->SetUniform(name, value);
		}

		static inline void SetUniform(std::string const &name, float value)
		{
			_API->SetUniform(name, value);
		}

		static inline void SetUniform(std::string const &name, glm::vec2 value)
		{
			_API->SetUniform(name, value);
		}

		static inline void SetUniform(std::string const &name, glm::vec3 value)
		{
			_API->SetUniform(name, value);
		}

		static inline void SetUniform(std::string const &name, glm::vec4 value)
		{
			_API->SetUniform(name, value);
		}

		static inline void SetUniform(std::string const &name, glm::mat3 value)
		{
			_API->SetUniform(name, value);
		}

		static inline void SetUniform(std::string const &name, glm::mat4 value)
		{
			_API->SetUniform(name, value);
		}

		static inline void SetUniform(std::string const &name, std::vector<glm::mat4> matrices, std::optional<size_t> size = std::nullopt)
		{
			_API->SetUniform(name, matrices, size);
		}

		static inline void SetUniform(std::string const &name, std::vector<glm::vec3> vectors, std::optional<size_t> size = std::nullopt)
		{
			_API->SetUniform(name, vectors, size);
		}

		static inline void SetDepthMask(bool val)
		{
			_API->SetDepthMask(val);
		}

};

}
