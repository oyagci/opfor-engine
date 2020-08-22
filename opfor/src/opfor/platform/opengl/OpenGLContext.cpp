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
	OP4_CORE_ASSERT(status, "Failed to initialize GLEW\n");

	int width = 0, height = 0;
	glfwGetWindowSize(_WindowHandle, &width, &height);

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void OpenGLContext::SwapBuffers()
{
	glfwPollEvents();
	glfwSwapBuffers(_WindowHandle);
}

}
