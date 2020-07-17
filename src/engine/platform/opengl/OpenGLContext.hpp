#pragma once

#include "engine/renderer/Context.hpp"

class GLFWwindow;

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
