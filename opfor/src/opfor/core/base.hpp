#pragma once

#include <memory>
#include <csignal>
#include <optional>
#include <variant>
#include <unordered_map>

#include "opfor/core/log.hpp"

#ifdef OP4_PLATFORM_LINUX
# define OP4_DEBUGBREAK() raise(SIGTRAP)
# define OP4_ABORT() abort();
#elif OP4_PLATFORM_WINDOWS
# include <Windows.h>
# define OP4_DEBUGBREAK() (void)0
# define OP4_ABORT() abort();
#else
# error "Unsupported Platform!"
# define OP4_DEBUGBREAK() (void)0
# define OP4_ABORT() (void)0
#endif

#define OP4_CORE_ASSERT(x, ...) { if (!(x)) { OP4_CORE_ERROR("Assertion Failed: {0}\n", __VA_ARGS__); OP4_DEBUGBREAK(); } }
#define OP4_CORE_EXPECT(x, ...) { if (!(x)) { OP4_CORE_ERROR("Assertion Failed: {0}\n", __VA_ARGS__); OP4_ABORT(); } }
#define OP4_CORE_UNREACHABLE(...) { OP4_CORE_ERROR("Unreachable: {0}\n"); OP4_ABORT(); }

#define BIT(x) (1 << x)

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace opfor {

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T, class Deleter = std::default_delete<T>>
using UniquePtr = std::unique_ptr<T, Deleter>;

template <typename T, typename ... Args>
constexpr SharedPtr<T> MakeShared(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T, typename ... Args>
constexpr UniquePtr<T> MakeUnique(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Optional = std::optional<T>;

template <typename T, typename Allocator = std::allocator<T>>
using Vector = std::vector<T, Allocator>;

template <typename T, std::size_t N>
using Array = std::array<T, N>;

using String = std::string;

template <typename ... T>
using Variant = std::variant<T...>;

template <typename Key, typename T>
using UnorderedMap = std::unordered_map<Key, T>;

}
