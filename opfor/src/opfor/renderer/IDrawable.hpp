#pragma once

class IDrawable
{
  public:
    virtual ~IDrawable()
    {
    }
    virtual void Draw() const = 0;
};
