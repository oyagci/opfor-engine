#include "LinuxInput.hpp"

#include <opfor/core/Application.hpp>
#include <opfor/core/base.hpp>
#include <GLFW/glfw3.h>
#include <renderer.hpp>

namespace opfor
{

LinuxInput *LinuxInput::_UniqueInstance = nullptr;

const std::unordered_map<int, KeyCode> LinuxInput::_GlfwKeycodes = {

    {GLFW_KEY_SPACE, KeyCode::Space},

    {GLFW_KEY_APOSTROPHE, KeyCode::Apostrophe},
    {GLFW_KEY_COMMA, KeyCode::Comma},
    {GLFW_KEY_MINUS, KeyCode::Minus},
    {GLFW_KEY_PERIOD, KeyCode::Period},
    {GLFW_KEY_SLASH, KeyCode::Slash},

    {GLFW_KEY_0, KeyCode::Num0},
    {GLFW_KEY_1, KeyCode::Num1},
    {GLFW_KEY_2, KeyCode::Num2},
    {GLFW_KEY_3, KeyCode::Num3},
    {GLFW_KEY_4, KeyCode::Num4},
    {GLFW_KEY_5, KeyCode::Num5},
    {GLFW_KEY_6, KeyCode::Num6},
    {GLFW_KEY_7, KeyCode::Num7},
    {GLFW_KEY_8, KeyCode::Num8},
    {GLFW_KEY_9, KeyCode::Num9},

    {GLFW_KEY_SEMICOLON, KeyCode::Semicolon},
    {GLFW_KEY_EQUAL, KeyCode::Equal},

    {GLFW_KEY_A, KeyCode::A},
    {GLFW_KEY_B, KeyCode::B},
    {GLFW_KEY_C, KeyCode::C},
    {GLFW_KEY_D, KeyCode::D},
    {GLFW_KEY_E, KeyCode::E},
    {GLFW_KEY_F, KeyCode::F},
    {GLFW_KEY_G, KeyCode::G},
    {GLFW_KEY_H, KeyCode::H},
    {GLFW_KEY_I, KeyCode::I},
    {GLFW_KEY_J, KeyCode::J},
    {GLFW_KEY_K, KeyCode::K},
    {GLFW_KEY_L, KeyCode::L},
    {GLFW_KEY_M, KeyCode::M},
    {GLFW_KEY_N, KeyCode::N},
    {GLFW_KEY_O, KeyCode::O},
    {GLFW_KEY_P, KeyCode::P},
    {GLFW_KEY_Q, KeyCode::Q},
    {GLFW_KEY_R, KeyCode::R},
    {GLFW_KEY_S, KeyCode::S},
    {GLFW_KEY_T, KeyCode::T},
    {GLFW_KEY_U, KeyCode::U},
    {GLFW_KEY_V, KeyCode::V},
    {GLFW_KEY_W, KeyCode::W},
    {GLFW_KEY_Y, KeyCode::Y},
    {GLFW_KEY_X, KeyCode::X},
    {GLFW_KEY_Z, KeyCode::Z},

    {GLFW_KEY_LEFT_BRACKET, KeyCode::LeftBracket},
    {GLFW_KEY_BACKSLASH, KeyCode::Backslash},
    {GLFW_KEY_RIGHT_BRACKET, KeyCode::RightBracket},
    {GLFW_KEY_GRAVE_ACCENT, KeyCode::GraveAccent},

    {GLFW_KEY_ESCAPE, KeyCode::Escape},
    {GLFW_KEY_ENTER, KeyCode::Enter},
    {GLFW_KEY_TAB, KeyCode::Tab},
    {GLFW_KEY_BACKSPACE, KeyCode::Backspace},

    {GLFW_KEY_INSERT, KeyCode::Insert},
    {GLFW_KEY_DELETE, KeyCode::Del},

    /* Directional Keys */
    {GLFW_KEY_RIGHT, KeyCode::Right},
    {GLFW_KEY_LEFT, KeyCode::Left},
    {GLFW_KEY_DOWN, KeyCode::Down},
    {GLFW_KEY_UP, KeyCode::Up},

    {GLFW_KEY_PAGE_UP, KeyCode::PageUp},
    {GLFW_KEY_PAGE_DOWN, KeyCode::PageDown},
    {GLFW_KEY_HOME, KeyCode::Home},
    {GLFW_KEY_END, KeyCode::End},

    {GLFW_KEY_CAPS_LOCK, KeyCode::CapsLock},
    {GLFW_KEY_SCROLL_LOCK, KeyCode::ScrollLock},
    {GLFW_KEY_NUM_LOCK, KeyCode::NumLock},
    {GLFW_KEY_PRINT_SCREEN, KeyCode::PrintScreen},
    {GLFW_KEY_PAUSE, KeyCode::Pause},

    {GLFW_KEY_F1, KeyCode::F1},
    {GLFW_KEY_F2, KeyCode::F2},
    {GLFW_KEY_F3, KeyCode::F3},
    {GLFW_KEY_F4, KeyCode::F4},
    {GLFW_KEY_F5, KeyCode::F5},
    {GLFW_KEY_F6, KeyCode::F6},
    {GLFW_KEY_F7, KeyCode::F7},
    {GLFW_KEY_F8, KeyCode::F8},
    {GLFW_KEY_F9, KeyCode::F9},
    {GLFW_KEY_F10, KeyCode::F10},
    {GLFW_KEY_F11, KeyCode::F11},
    {GLFW_KEY_F12, KeyCode::F12},
    {GLFW_KEY_F13, KeyCode::F13},
    {GLFW_KEY_F14, KeyCode::F14},

    /* Keypad */
    {GLFW_KEY_KP_0, KeyCode::Numpad0},
    {GLFW_KEY_KP_1, KeyCode::Numpad1},
    {GLFW_KEY_KP_2, KeyCode::Numpad2},
    {GLFW_KEY_KP_3, KeyCode::Numpad3},
    {GLFW_KEY_KP_4, KeyCode::Numpad4},
    {GLFW_KEY_KP_5, KeyCode::Numpad5},
    {GLFW_KEY_KP_6, KeyCode::Numpad6},
    {GLFW_KEY_KP_7, KeyCode::Numpad7},
    {GLFW_KEY_KP_8, KeyCode::Numpad8},
    {GLFW_KEY_KP_9, KeyCode::Numpad9},

    {GLFW_KEY_KP_DECIMAL, KeyCode::NumpadDecimal},
    {GLFW_KEY_KP_DIVIDE, KeyCode::NumpadDivide},
    {GLFW_KEY_KP_MULTIPLY, KeyCode::NumpadMultiply},
    {GLFW_KEY_KP_SUBTRACT, KeyCode::NumpadSubtract},
    {GLFW_KEY_KP_ADD, KeyCode::NumpadAdd},
    {GLFW_KEY_KP_ENTER, KeyCode::NumpadEnter},
    {GLFW_KEY_KP_EQUAL, KeyCode::NumpadEqual},

    {GLFW_KEY_LEFT_SHIFT, KeyCode::LeftShift},
    {GLFW_KEY_LEFT_CONTROL, KeyCode::LeftControl},
    {GLFW_KEY_LEFT_ALT, KeyCode::LeftAlt},
    {GLFW_KEY_LEFT_SUPER, KeyCode::LeftSuper},

    {GLFW_KEY_RIGHT_SHIFT, KeyCode::RightShift},
    {GLFW_KEY_RIGHT_CONTROL, KeyCode::RightControl},
    {GLFW_KEY_RIGHT_ALT, KeyCode::RightAlt},
    {GLFW_KEY_RIGHT_SUPER, KeyCode::RightSuper},

    {GLFW_KEY_MENU, KeyCode::Menu},
};

const std::unordered_map<int, MouseButton> LinuxInput::_GlfwMouseButtons = {
    {GLFW_MOUSE_BUTTON_1, MouseButton::Button1}, {GLFW_MOUSE_BUTTON_2, MouseButton::Button2},
    {GLFW_MOUSE_BUTTON_3, MouseButton::Button3}, {GLFW_MOUSE_BUTTON_4, MouseButton::Button4},
    {GLFW_MOUSE_BUTTON_5, MouseButton::Button5}, {GLFW_MOUSE_BUTTON_6, MouseButton::Button6},
    {GLFW_MOUSE_BUTTON_7, MouseButton::Button7}, {GLFW_MOUSE_BUTTON_8, MouseButton::Button8},
};

LinuxInput::LinuxInput()
{
    OP4_CORE_ASSERT(_UniqueInstance == nullptr, "LinuxInput already exists. Only instanciate LinuxInput once!");

    _UniqueInstance = this;

    _WindowHandle = reinterpret_cast<GLFWwindow *>(Application::Get().GetWindow()->GetRawHandle());

    glfwSetKeyCallback(_WindowHandle, &LinuxInput::KeyCallback);
    glfwSetMouseButtonCallback(_WindowHandle, &LinuxInput::MouseButtonCallback);
    glfwSetCursorPosCallback(_WindowHandle, &LinuxInput::MouseCallback);
}

void LinuxInput::RegisterKey(int action, int key, int mods)
{
    KeyCode keyCode = GetGLFWKeyCode(key);

    if (keyCode == KeyCode::None)
    {
        return;
    }

    if (action == GLFW_PRESS)
    {
        RegisterKeyPress(keyCode, mods);
    }
    else if (action == GLFW_RELEASE)
    {
        RegisterKeyRelease(keyCode, mods);
    }
}

void LinuxInput::RegisterKeyPress(KeyCode keyCode, int /* mods */)
{
    _PressedKeys.insert(keyCode);
}

void LinuxInput::RegisterKeyRelease(KeyCode keyCode, int /* mods */)
{
    _ReleasedKeys.insert(keyCode);
}

void LinuxInput::RegisterMousePos(float x, float y)
{
    _LastMousePosition = _MousePosition;
    _MousePosition = {x, y};
}

void LinuxInput::RegisterMouseButton(int glfwButton, int action, int mods)
{
    auto button = GetGLFWMouseButton(glfwButton);

    if (button == MouseButton::None)
    {
        return;
    }

    if (action == GLFW_PRESS)
    {
        RegisterMouseButtonPress(button, mods);
    }
    else if (action == GLFW_RELEASE)
    {
        RegisterMouseButtonRelease(button, mods);
    }
}

void LinuxInput::RegisterMouseButtonPress(MouseButton button, int /* mods */)
{
    _PressedMouseButtons.insert(button);
}

void LinuxInput::RegisterMouseButtonRelease(MouseButton button, int /* mods */)
{
    _ReleasedMouseButtons.insert(button);
}

KeyCode LinuxInput::GetGLFWKeyCode(int key) const
{
    auto keycode = _GlfwKeycodes.find(key);

    if (keycode != _GlfwKeycodes.end())
    {
        return keycode->second;
    }

    return KeyCode::None;
}

MouseButton LinuxInput::GetGLFWMouseButton(int glfwButton) const
{
    auto button = _GlfwMouseButtons.find(glfwButton);

    if (button != _GlfwMouseButtons.end())
    {
        return button->second;
    }

    return MouseButton::None;
}

bool LinuxInput::GetKeyDown_Impl(KeyCode keyCode)
{
    return _PressedKeys.find(keyCode) != _PressedKeys.end();
}

bool LinuxInput::GetKeyUp_Impl(KeyCode keyCode)
{
    return _ReleasedKeys.find(keyCode) != _ReleasedKeys.end();
}

KeyStatus LinuxInput::GetKey_Impl(KeyCode keyCode)
{
    if (glfwGetKey(_WindowHandle, (int)keyCode) == GLFW_PRESS)
    {
        return KeyStatus::Pressed;
    }
    else
    {
        return KeyStatus::Released;
    }
}

glm::vec2 LinuxInput::GetMousePosition_Impl()
{
    return _MousePosition;
}

glm::vec2 LinuxInput::GetMouseRelativePosition_Impl()
{
    auto rel = _LastMousePosition - _MousePosition;

    // Reset the value of _LastMousePosition to prevent ghost inputs from previous frame
    _LastMousePosition = _MousePosition;

    return rel;
}

bool LinuxInput::GetMouseButtonDown_Impl(MouseButton btn)
{
    return _PressedMouseButtons.find(btn) != _PressedMouseButtons.end();
}

bool LinuxInput::GetMouseButtonUp_Impl(MouseButton btn)
{
    return _ReleasedMouseButtons.find(btn) != _ReleasedMouseButtons.end();
}

KeyStatus LinuxInput::GetMouseButton_Impl(MouseButton btn)
{
    if (glfwGetMouseButton(_WindowHandle, static_cast<int>(btn)) == GLFW_PRESS)
    {
        return KeyStatus::Pressed;
    }
    else
    {
        return KeyStatus::Released;
    }
}

void LinuxInput::Flush_Impl()
{
    _PressedKeys.clear();
    _ReleasedKeys.clear();
    _PressedMouseButtons.clear();
    _ReleasedMouseButtons.clear();
}

void LinuxInput::KeyCallback(GLFWwindow *, int key, int /* scancode */, int action, int mods)
{
    _UniqueInstance->RegisterKey(action, key, mods);
}

void LinuxInput::MouseCallback(GLFWwindow *, double xpos, double ypos)
{
    _UniqueInstance->RegisterMousePos(static_cast<float>(xpos), static_cast<float>(ypos));
}

void LinuxInput::MouseButtonCallback(GLFWwindow *, int button, int action, int mods)
{
    _UniqueInstance->RegisterMouseButton(button, action, mods);
}

} // namespace opfor
