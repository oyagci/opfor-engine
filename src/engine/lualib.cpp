#include "lualib.h"
#include "lua.hpp"
#include <fmt/format.h>
#include "Engine.hpp"
#include "components/TransformComponent.hpp"

static int Engine_setPosition(lua_State *L)
{
	lua_settop(L, 1);

	lua_getglobal(L, "__ENTITY_ID__");
	lua_Integer entityId = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	luaL_checktype(L, 1, LUA_TTABLE);

	auto entity = opfor::Engine::Instance().GetEntity(entityId);

	if (!entity.has_value()) { return 0; }
	if (!entity.value()->HasComponents<TransformComponent>()) { return 0; }

	lua_getfield(L, 1, "x");
	lua_getfield(L, 1, "y");
	lua_getfield(L, 1, "z");

	lua_Number x = luaL_checknumber(L, -3);
	lua_Number y = luaL_checknumber(L, -2);
	lua_Number z = luaL_checknumber(L, -1);

	lua_pop(L, 3);

	auto &transform = entity.value()->Get<TransformComponent>();
	transform.position = { x, y, z };

	return 0;
}

static int Engine_setScale(lua_State *L)
{
	lua_settop(L, 1);

	lua_getglobal(L, "__ENTITY_ID__");
	lua_Integer entityId = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	luaL_checktype(L, 1, LUA_TTABLE);

	auto entity = opfor::Engine::Instance().GetEntity(entityId);

	if (!entity.has_value()) { return 0; }
	if (!entity.value()->HasComponents<TransformComponent>()) { return 0; }

	lua_getfield(L, 1, "x");
	lua_getfield(L, 1, "y");
	lua_getfield(L, 1, "z");

	lua_Number x = luaL_checknumber(L, -3);
	lua_Number y = luaL_checknumber(L, -2);
	lua_Number z = luaL_checknumber(L, -1);

	lua_pop(L, 3);

	auto &transform = entity.value()->Get<TransformComponent>();
	transform.scale = { x, y, z };

	return 0;
}

int luaopen_Engine(lua_State *L)
{
	const luaL_Reg libFuncs[] = {
		{ "setPosition", Engine_setPosition },
		{ "setScale", Engine_setScale },
		{ nullptr, nullptr }
	};

	luaL_newlib(L, libFuncs);

	return 1;
}
