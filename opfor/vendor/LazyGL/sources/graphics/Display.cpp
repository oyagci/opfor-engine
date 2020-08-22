//
// Created by Marc on 01/06/2018.
//

#include <lazy.hpp>
#include "Display.hpp"
#include "inputs/Input.hpp"
#include "inputs/Keyboard.hpp"

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
	if (severity <= GL_DEBUG_SEVERITY_NOTIFICATION) return ;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}


namespace lazy
{
	namespace graphics
	{
		Display::Display(const std::string &title, int width, int height)
			: title(title), width(width), height(height), resized(false)
		{
			if (!glfwInit())
				throw std::runtime_error("GLFW error: Unable to init glfw !");

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
//			glfwWindowHint(GLFW_SAMPLES, 4);

			window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
			if (!window)
				throw std::runtime_error("GLFW error: Unable to create window !");

			glfwMakeContextCurrent(window);
			glViewport(0, 0, width, height);

			glewExperimental = GL_TRUE;
			if (glewInit() != GLEW_OK)
				throw std::runtime_error("GLEW error: Unable to init glew !");

			std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

//			glEnable(GL_DEBUG_OUTPUT);
//			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
//			glDebugMessageCallback(glDebugOutput, nullptr);
//			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

			inputs::input::init(*this);

			updateScreenSize();
		}

		Display::~Display()
		{
			inputs::input::clean();
			glfwTerminate();
		}

		void Display::update()
		{
			int newWidth;
			int newHeight;

			glfwSwapBuffers(window);
			glfwPollEvents();

			resized = false;
			glfwGetWindowSize(window, &newWidth, &newHeight);
			if (newWidth != width || newHeight != height)
			{
				resized = true;
				width = newWidth;
				height = newHeight;
				this->updateViewport();
			}
		}

		void Display::updateInputs()
		{
			inputs::input::update();
		}

		void Display::updateViewport()
		{
			glViewport(0, 0, width, height);
		}

		Display &Display::enableCap(const GLenum &cap)
		{
			glEnable(cap);

			return *this;
		}

		void Display::setFullscreen(bool fullscreen)
		{
			if (fullscreen)
			{
				int nmonitors = 0;
				GLFWmonitor **monitors = glfwGetMonitors(&nmonitors);

				if (nmonitors > 0)
				{
					const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[0]);
					glfwSetWindowMonitor(window, monitors[0], 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
					isFullscreen = true;
				}
			}
			else
			{
				glfwSetWindowMonitor(window, 0, 0, 0, width, height, 0);
				isFullscreen = false;
			}
		}

		void Display::showCursor(bool show)
		{
			if (show)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}

		void Display::updateScreenSize()
		{
			int nmonitors = 0;
			GLFWmonitor **monitors = glfwGetMonitors(&nmonitors);

			if (nmonitors > 0)
			{
				const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[0]);
				screenSize.x = vidmode->width;
				screenSize.y = vidmode->height;
			}
		}
	}
}
