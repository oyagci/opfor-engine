#pragma once

enum class Platform
{
    None,
    Linux
};

#ifdef OP4_PLATFORM_LINUX
static constexpr Platform OP4_CURRENT_PLATFORM
#else
#error "Unsupported platform!"
#endif
