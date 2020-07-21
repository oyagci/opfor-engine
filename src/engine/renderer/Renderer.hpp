#pragma once

#include "RenderCommand.hpp"

namespace opfor {

enum class ClearFlags
{
	ColorBit,
	DepthBit,
};

class Renderer
{
public:
	static void BeginScene();
	static void EndScene();

	static void SetClearColor(std::array<float, 4> const &color);
	static void Clear();

	static void Submit(SharedPtr<VertexArray> const &);

	static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};

}
