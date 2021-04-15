#pragma once

#include <opfor/core/Input.hpp>
#include <renderer.hpp>
#include <unordered_map>
#include <unordered_set>

struct GLFWwindow;

namespace opfor
{

class LinuxInput : public IInput
{
  private:
    static const std::unordered_map<int, KeyCode> _GlfwKeycodes;
    static const std::unordered_map<int, MouseButton> _GlfwMouseButtons;

    GLFWwindow *_WindowHandle = nullptr;

    std::unordered_set<KeyCode> _PressedKeys;
    std::unordered_set<KeyCode> _ReleasedKeys;
    std::unordered_set<MouseButton> _PressedMouseButtons;
    std::unordered_set<MouseButton> _ReleasedMouseButtons;
    Vec2 _LastMousePosition = {0.0f, 0.0f};
    Vec2 _MousePosition = {0.0f, 0.0f};

    static LinuxInput *_UniqueInstance;

    void RegisterKey(int action, int key, int mods);
    void RegisterKeyPress(KeyCode keyCode, int mods);
    void RegisterKeyRelease(KeyCode keyCode, int mods);

    void RegisterMousePos(float xpos, float ypos);

    void RegisterMouseButtonPress(MouseButton button, int mods);
    void RegisterMouseButtonRelease(MouseButton button, int mods);
    void RegisterMouseButton(int button, int action, int mods);

    KeyCode GetGLFWKeyCode(int key) const;
    MouseButton GetGLFWMouseButton(int glfwButton) const;

  public:
    LinuxInput();
    LinuxInput(LinuxInput const &LinuxInput) = delete;
    LinuxInput &operator=(LinuxInput const &) = delete;
    ~LinuxInput() = default;

  protected:
    bool GetKeyDown_Impl(KeyCode keyCode) override;
    bool GetKeyUp_Impl(KeyCode keyCode) override;
    KeyStatus GetKey_Impl(KeyCode keyCode) override;

    Vec2 GetMousePosition_Impl() override;
    Vec2 GetMouseRelativePosition_Impl() override;

    bool GetMouseButtonDown_Impl(MouseButton btn) override;
    bool GetMouseButtonUp_Impl(MouseButton btn) override;

    KeyStatus GetMouseButton_Impl(MouseButton button) override;

    void Flush_Impl() override;

    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow *window, double, double);
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
};

} // namespace opfor
