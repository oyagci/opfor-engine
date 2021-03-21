#pragma once

#include "opfor/renderer/Context.hpp"


namespace opfor
{

class IWindow;

class OpenGLContext : public IRendererContext
{
  private:
    IWindow &_WindowHandle;

  public:
    OpenGLContext() = delete;
    OpenGLContext(OpenGLContext &) = delete;

    OpenGLContext(IWindow &);

    void Init() override;
    void SwapBuffers() override;
};

} // namespace opfor
