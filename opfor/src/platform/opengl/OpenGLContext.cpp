#include "OpenGLContext.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace opfor {

OpenGLContext::OpenGLContext(GLFWwindow *window)
	: _WindowHandle(window)
{
	OP4_CORE_ASSERT(_WindowHandle, "Window handle is null\n");
}

void OpenGLContext::Init()
{
	glfwMakeContextCurrent(_WindowHandle);

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	OP4_CORE_ASSERT(status, "Failed to initialize GLAD\n");

	int width = 0, height = 0;
	glfwGetWindowSize(_WindowHandle, &width, &height);

	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	OP4_CORE_INFO("OpenGL Info:\n");
	OP4_CORE_INFO("  Vendor: {}\n", glGetString(GL_VENDOR));
	OP4_CORE_INFO("  Renderer: {}\n", glGetString(GL_RENDERER));
	OP4_CORE_INFO("  Version: {}\n", glGetString(GL_VERSION));
}

void OpenGLContext::SwapBuffers()
{
	glfwPollEvents();
	glfwSwapBuffers(_WindowHandle);
}

}
