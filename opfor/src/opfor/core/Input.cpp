#include "Input.hpp"
#include "opfor/platform/linux/LinuxInput.hpp"

namespace opfor {

UniquePtr<IInput> Input::Create()
{
#ifdef OP4_PLATFORM_LINUX
	return MakeUnique<LinuxInput>();
#endif

	OP4_CORE_ASSERT(false, "Unsupported Input Platform");
	abort();
}

}
