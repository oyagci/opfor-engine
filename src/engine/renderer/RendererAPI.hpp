#pragma once

#include <array>
#include "engine/core/base.hpp"
#include "engine/renderer/VertexArray.hpp"

namespace opfor {

class RendererAPI
{
public:
	enum class API {
		None = 0,
		OpenGL = 1,
	};

private:
	static API _API;

public:
	virtual ~RendererAPI() = default;

	virtual void Clear() = 0;
	virtual void SetClearColor(std::array<float, 4>) = 0;

	virtual void DrawIndexed(SharedPtr<VertexArray> const &) = 0;

	inline static API GetAPI() { return _API; }
};

}
