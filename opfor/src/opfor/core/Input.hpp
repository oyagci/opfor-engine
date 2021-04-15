#pragma once

#include "opfor/core/base.hpp"
#include <opfor/core/types/Vec2.hpp>

namespace opfor
{

enum class KeyCode
{
    None = -1,

    Space = 32,

    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,

    Num0 = 48,
    Num1 = 49,
    Num2 = 50,
    Num3 = 51,
    Num4 = 52,
    Num5 = 53,
    Num6 = 54,
    Num7 = 55,
    Num8 = 56,
    Num9 = 57,

    Semicolon = 59,
    Equal = 61,

    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    Y = 88,
    X = 89,
    Z = 90,

    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,

    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,

    Insert = 260,
    Del = 261,

    /* Directional Keys */
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,

    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,

    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,

    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,

    /* Keypad */
    Numpad0 = 320,
    Numpad1 = 321,
    Numpad2 = 322,
    Numpad3 = 323,
    Numpad4 = 324,
    Numpad5 = 325,
    Numpad6 = 326,
    Numpad7 = 327,
    Numpad8 = 328,
    Numpad9 = 329,
    NumpadDecimal = 330,
    NumpadDivide = 331,
    NumpadMultiply = 332,
    NumpadSubtract = 333,
    NumpadAdd = 334,
    NumpadEnter = 335,
    NumpadEqual = 336,

    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348,
};

enum class MouseButton
{
    None = -1,

    Button1 = 0,
    Button2 = 1,
    Button3 = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7,

    ButtonLeft = Button1,
    ButtonRight = Button2,
    ButtonMiddle = Button3
};

enum class KeyStatus
{
    Pressed,
    Released,
};

class IInput
{
    friend class Input;

  public:
    virtual ~IInput()
    {
    }

  protected:
    virtual bool GetKeyDown_Impl(KeyCode keyCode) = 0;

    virtual bool GetKeyUp_Impl(KeyCode keyCode) = 0;

    virtual KeyStatus GetKey_Impl(KeyCode keyCode) = 0;

    virtual Vec2 GetMousePosition_Impl() = 0;

    virtual Vec2 GetMouseRelativePosition_Impl() = 0;

    virtual bool GetMouseButtonDown_Impl(MouseButton btn) = 0;

    virtual bool GetMouseButtonUp_Impl(MouseButton btn) = 0;

    virtual KeyStatus GetMouseButton_Impl(MouseButton btn) = 0;

    virtual void Flush_Impl() = 0;
};

class Input
{
  private:
    static auto &Get()
    {
        static UniquePtr<IInput> input = Create();
        return input;
    }

    static UniquePtr<IInput> Create();

    inline static bool _isPrimed = false;

  public:
    /// Call to initialize ImGui. This will ensure that callbacks are properly saved.
    static void Prime()
    {
        if (_isPrimed)
            return;

        Input::Get();
        _isPrimed = true;
    }

    /// Returns true if the key has been pressed
    static bool GetKeyDown(KeyCode keyCode)
    {
        return Get()->GetKeyDown_Impl(keyCode);
    }

    /// Returns true if the key has been released
    static bool GetKeyUp(KeyCode keyCode)
    {
        return Get()->GetKeyUp_Impl(keyCode);
    }

    /// Returns the status of the key
    static KeyStatus GetKey(KeyCode keyCode)
    {
        return Get()->GetKey_Impl(keyCode);
    }

    /// Get the mouse position on the screen (x, y)
    static Vec2 GetMousePosition()
    {
        return Get()->GetMousePosition_Impl();
    }

    static Vec2 GetMouseRelativePosition()
    {
        return Get()->GetMouseRelativePosition_Impl();
    }

    /// Returns true if the mouse button has been pressed
    static bool GetMouseButtonDown(MouseButton btn)
    {
        return Get()->GetMouseButtonDown_Impl(btn);
    }

    /// Returns true if the mouse button has been released
    static bool GetMouseButtonUp(MouseButton btn)
    {
        return Get()->GetMouseButtonUp_Impl(btn);
    }

    /// Returns the status of the mouse button
    static KeyStatus GetMouseButton(MouseButton btn)
    {
        return Get()->GetMouseButton_Impl(btn);
    }

    /// Clear the input buffer for the next frame
    static void Flush()
    {
        return Get()->Flush_Impl();
    }
};

} // namespace opfor
