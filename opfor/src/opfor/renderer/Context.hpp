#pragma once

#include "opfor/core/base.hpp"

struct GLFWwindow;

namespace opfor
{

class IRendererContext
{
  private:
  public:
    virtual ~IRendererContext()
    {
    }

    virtual void Init() = 0;
    virtual void SwapBuffers() = 0;

    static UniquePtr<IRendererContext> Create(GLFWwindow*);
};

} // namespace opfor
