#pragma once
#include "opfor/core/base.hpp"

namespace opfor
{

class IWindow;

class IRendererContext
{
  private:
  public:
    virtual ~IRendererContext() = default;

    virtual void Init() = 0;
    virtual void SwapBuffers() = 0;

    static UniquePtr<IRendererContext> Create(IWindow &);
};

} // namespace opfor
