#include "Input.hpp"
#include "platform/linux/LinuxInput.hpp"
#include "platform/windows/WindowsInput.hpp"

namespace opfor
{

UniquePtr<IInput> Input::Create()
{
#ifdef OP4_PLATFORM_LINUX
    return MakeUnique<LinuxInput>();
#elif defined(OP4_PLATFORM_WINDOWS)
    return MakeUnique<WindowsInput>();
#endif

    OP4_CORE_ASSERT(false, "Unsupported Input Platform");
    abort();
}

} // namespace opfor
