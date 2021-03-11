#pragma once

#include <functional>

class BaseCallback
{
  public:
    virtual ~BaseCallback()
    {
    }
};

template <typename... Args> class Callback : public BaseCallback
{
  private:
    typedef std::function<void(Args...)> FuncType;
    FuncType _f;

  public:
    Callback()
    {
    }
    Callback(FuncType f)
    {
        _f = f;
    }

    void operator()(Args... args) noexcept(std::is_nothrow_invocable<decltype(_f)>::value)
    {
        if (_f)
        {
            _f(args...);
        }
    }

    void operator=(FuncType f)
    {
        _f = f;
    }
};
