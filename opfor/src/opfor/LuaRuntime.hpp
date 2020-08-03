#pragma once

#include <exception>
#include <string>
#include <functional>
#include <memory>
#include <fmt/format.h>
#include "lua.hpp"
#include "lualib.h"
#include "opfor/core/Logger.hpp"

struct lua_StateDeleter
{
	void operator()(lua_State *L) noexcept
	{
		if (L != nullptr) {
			lua_close(L);
		}
	}
};

class LuaRuntime
{
private:
	std::unique_ptr<lua_State, lua_StateDeleter> _state;

public:
	LuaRuntime()
	{
		lua_State *L = luaL_newstate();

		_state = std::unique_ptr<lua_State, lua_StateDeleter>(L);

		luaL_openlibs(_state.get());
	}

	void RegisterFunction(std::string const &funcName, int (*func)(lua_State *))
	{
		lua_register(_state.get(), funcName.c_str(), func);
	}

	bool Load(std::string const &path)
	{
		auto *const state = _state.get();

		if (luaL_loadfile(state, path.c_str()) != LUA_OK) {
			return false;
		}

		/* Load engine library */
		luaL_requiref(state, "Engine", &luaopen_Engine, true);
		lua_pop(state, 1);

		/* Priming Call -- Mandatory */
		lua_pcall(state, 0, 0, 0);

		return true;
	}

	void Reset()
	{
		if (_state) {
			_state.reset(luaL_newstate());
			luaL_openlibs(_state.get());
		}
	}

	void PushArg(float arg) const
	{
		lua_pushnumber(_state.get(), arg);
	}

	void PushArg(int arg) const
	{
		lua_pushinteger(_state.get(), static_cast<lua_Integer>(arg));
	}

	void PushArgs() const {}

	template <typename T, typename ... ArgTypes>
	void PushArgs(T arg, ArgTypes ... args) const
	{
		PushArg(arg);
		PushArgs(std::forward<ArgTypes>(args)...);
	}

	template <typename ... ArgTypes>
	void Call(std::string const &funcName, ArgTypes ... args) const
	{
		size_t nbArgs = sizeof...(ArgTypes);

		lua_getglobal(_state.get(), funcName.c_str());
		if (nbArgs > 0) {
			PushArgs(std::forward<ArgTypes>(args)...);
		}

		if (lua_pcall(_state.get(), nbArgs, 0, 0) != LUA_OK) {
			const char *err = lua_tostring(_state.get(), -1);
			Logger::Error("Runtime Error: {}\n", err);
		}
	}

	void PushGlobal(std::string const &name, int value)
	{
		lua_pushinteger(_state.get(), static_cast<lua_Integer>(value));
		lua_setglobal(_state.get(), name.c_str());
	}

	void PushGlobal(std::string const &name, unsigned int value)
	{
		lua_pushinteger(_state.get(), static_cast<lua_Integer>(value));
		lua_setglobal(_state.get(), name.c_str());
	}

	void PushGlobal(std::string const &name, float value)
	{
		lua_pushnumber(_state.get(), static_cast<lua_Number>(value));
		lua_setglobal(_state.get(), name.c_str());
	}
};
