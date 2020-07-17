#include "Window.hpp"
#include "engine/platform/linux/LinuxWindow.hpp"

namespace opfor {

UniquePtr<IWindow> IWindow::Create(const WindowProps &props)
{
#ifdef OP4_PLATFORM_LINUX
	return MakeUnique<LinuxWindow>(props);
#endif

	OP4_CORE_ASSERT(false, "Unsupported Platform!");
	OP4_ABORT();
}

}
