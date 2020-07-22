#pragma once

#include "RendererAPI.hpp"

namespace opfor {

class RenderCommand
{
	private:
		static UniquePtr<RendererAPI> _API;

	public:
		static inline void SetClearColor(std::array<float, 4> const color)
		{
			_API->SetClearColor(std::forward<decltype(color)>(color));
		}

		static inline void Clear()
		{
			_API->Clear();
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
};

}
