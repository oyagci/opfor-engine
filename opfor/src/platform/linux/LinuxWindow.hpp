#pragma once

#include <opfor/core/Window.hpp>

struct GLFWwindow;

namespace opfor
{

class LinuxWindow : public IWindow
{
  private:
    GLFWwindow *_Window;

    struct WindowData
    {
        std::string Title;
        int Width;
        int Height;

        EventCallbackFn EventCallback;
    };

    WindowData _Data;

  public:
    LinuxWindow() = delete;
    LinuxWindow(const WindowProps &props);

    void HideCursor(bool hide) const override;
    void *GetRawHandle() const override;
    void Close() const override;

    int GetWidth() const override;
    int GetHeight() const override;

    bool IsClosed() const override;

    void SetEventCallback(const EventCallbackFn &eventCallback) override
    {
        _Data.EventCallback = eventCallback;
    }
};

} // namespace opfor
