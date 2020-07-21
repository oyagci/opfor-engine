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
};

}
