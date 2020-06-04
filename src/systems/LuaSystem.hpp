#pragma once

#include "ecs/System.hpp"
#include "components/LuaScriptComponent.hpp"

class LuaSystem : public ecs::ComponentSystem
{
private:
public:
	void OnUpdate(float deltaTime) override
	{
		auto luaScripts = GetEntities<LuaScriptComponent>();

		for (auto const &script : luaScripts) {
			auto [ lua ] = script->GetAll();

			lua.Runtime.Call("onUpdate", deltaTime);
		}
	}
};
