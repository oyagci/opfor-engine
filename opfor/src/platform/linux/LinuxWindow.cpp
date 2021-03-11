#include "LinuxWindow.hpp"
#include "core/events/EngineEvents.hpp"

#include <GLFW/glfw3.h>

namespace opfor
{

LinuxWindow::LinuxWindow(const WindowProps &props)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Error: GLFW: Initialization failed!");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    _Window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(_Window, &_Data);

    if (!_Window)
    {
        throw std::runtime_error("Error: GLFW: Window creation failed!");
    }

    glfwSetWindowSizeCallback(_Window, [](GLFWwindow *window, int width, int height) {
        WindowResizeEvent event(width, height);

        WindowData *data = reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));
        data->EventCallback(event);
        data->Width = width;
        data->Height = height;
    });

    glfwSetWindowCloseCallback(_Window, [](GLFWwindow *window) {
        WindowCloseEvent event;

        WindowData *data = reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));
        data->EventCallback(event);
    });
}

void LinuxWindow::HideCursor(bool hide) const
{
    if (hide)
    {
        glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
        glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void *LinuxWindow::GetRawHandle() const
{
    return _Window;
}

void LinuxWindow::Close() const
{
    glfwSetWindowShouldClose(_Window, GLFW_TRUE);
}

int LinuxWindow::GetWidth() const
{
    int width = 0;
    int height = 0;

    glfwGetWindowSize(_Window, &width, &height);

    return width;
}

int LinuxWindow::GetHeight() const
{
    int width = 0;
    int height = 0;

    glfwGetWindowSize(_Window, &width, &height);

    return height;
}

bool LinuxWindow::IsClosed() const
{
    return glfwWindowShouldClose(_Window) == GLFW_TRUE;
}

} // namespace opfor
