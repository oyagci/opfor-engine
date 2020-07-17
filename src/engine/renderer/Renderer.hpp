#pragma once

namespace opfor {

enum class RendererAPI {
	None = 0,
	OpenGL = 1,
};

class Renderer
{
private:
	static RendererAPI _RendererAPI;

public:
	static RendererAPI GetAPI() { return _RendererAPI; }
};

}
