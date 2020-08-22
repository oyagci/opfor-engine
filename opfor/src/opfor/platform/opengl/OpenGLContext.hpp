#pragma once

#include "opfor/renderer/Context.hpp"

struct GLFWwindow;

namespace opfor {

class OpenGLContext : public IRendererContext
{
private:
	GLFWwindow *_WindowHandle;

public:
	OpenGLContext() = delete;
	OpenGLContext(GLFWwindow *window);
	OpenGLContext(OpenGLContext &) = delete;

	void Init() override;
	void SwapBuffers() override;
};

}
