#include "Window.hpp"
#ifdef OP4_PLATFORM_LINUX
#include "platform/linux/LinuxWindow.hpp"
#elif defined(OP4_PLATFORM_WINDOWS)
#include "platform/windows/WindowsWindow.hpp"
#endif

namespace opfor
{

UniquePtr<IWindow> IWindow::Create(const WindowProps &props)
{
#ifdef OP4_PLATFORM_LINUX
    return MakeUnique<LinuxWindow>(props);
#elif defined(OP4_PLATFORM_WINDOWS)
    return MakeUnique<WindowsWindow>(props);
#endif

    OP4_CORE_ASSERT(false, "Unsupported Platform!");
    OP4_ABORT();
}

} // namespace opfor
