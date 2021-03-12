#pragma once

class IDrawable
{
  public:
    virtual ~IDrawable() = default;
    virtual void Draw() const = 0;
};
