#pragma once

#include "engine/core/Window.hpp"

struct GLFWwindow;

namespace opfor {

class LinuxWindow : public IWindow
{
private:
	GLFWwindow *_Window;

public:
	LinuxWindow() = delete;
	LinuxWindow(const WindowProps &props);

	void HideCursor(bool hide) const;
	void *GetRawHandle() const;
	void Close() const;

	int GetWidth() const;
	int GetHeight() const;

	bool IsClosed() const;
};

}
