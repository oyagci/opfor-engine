#pragma once

#include "engine/renderer/RendererAPI.hpp"

namespace opfor {

class OpenGLRendererAPI : public RendererAPI
{
public:
	void Clear() override;
	void SetClearColor(std::array<float, 4> const) override;

	void DrawIndexed(SharedPtr<VertexArray> const &) override;
};

}
