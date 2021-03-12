#include "OpenGLContext.hpp"
#include "opfor/core/Window.hpp"
#include "opfor/core/base.hpp"

namespace opfor
{

UniquePtr<IRendererContext> IRendererContext::Create(IWindow &window)
{
    return MakeUnique<OpenGLContext>(window);
}

OpenGLContext::OpenGLContext(IWindow &window) : _WindowHandle(window)
{
}

void OpenGLContext::Init()
{
    // TODO remove glfw specific code and expose this method in the IWindow interface
    glfwMakeContextCurrent(static_cast<GLFWwindow *>(_WindowHandle.GetRawHandle()));

    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    OP4_CORE_ASSERT(status, "Failed to initialize GLAD\n");

   // const auto width = _WindowHandle.GetWidth();
   // const auto height = _WindowHandle.GetHeight();

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
    // TODO remove glfw specific code and expose this method in the IWindow interface
    glfwPollEvents();
    glfwSwapBuffers(static_cast<GLFWwindow *>(_WindowHandle.GetRawHandle()));
}

} // namespace opfor
