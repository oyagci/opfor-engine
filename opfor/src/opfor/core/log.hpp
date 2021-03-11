#pragma once

#include "Logger.hpp"

#define OP4_CORE_ERROR(...) Logger::Error(__VA_ARGS__)
#define OP4_CORE_WARNING(...) Logger::Warn(__VA_ARGS__)
#define OP4_CORE_DEBUG(...) Logger::Debug(__VA_ARGS__)
#define OP4_CORE_INFO(...) Logger::Info(__VA_ARGS__)
#define OP4_CORE_VERBOSE(...) Logger::Verbose(__VA_ARGS__)
