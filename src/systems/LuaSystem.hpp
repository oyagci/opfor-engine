#pragma once

#include "ecs/System.hpp"
#include "components/LuaScriptComponent.hpp"
#include "Engine.hpp"

namespace {

class LuaSystemState : public ecs::ComponentSystem
{
private:
public:
	virtual ~LuaSystemState() {}
	virtual void OnUpdate(float) override {}
};

class LuaSystemIdleState : public LuaSystemState
{
private:
public:
};

class LuaSystemPlayState : public LuaSystemState
{
public:
	void OnStart() override
	{
		auto luaScripts = GetEntities<LuaScriptComponent>();

		for (auto const &script : luaScripts) {
			auto [ lua ] = script->GetAll();

			lua.Runtime.Call("onStart");
		}
	}

	void OnUpdate(float deltaTime) override
	{
		auto luaScripts = GetEntities<LuaScriptComponent>();

		for (auto const &script : luaScripts) {
			auto [ lua ] = script->GetAll();

			lua.Runtime.Call("onUpdate", deltaTime);
		}
	}

	void OnStop() override
	{
		auto luaScripts = GetEntities<LuaScriptComponent>();

		for (auto const &script : luaScripts) {
			auto [ lua ] = script->GetAll();

			lua.Runtime.Call("onStop");
		}
	}
};

}

class LuaSystem : public ecs::ComponentSystem
{
private:
	std::unique_ptr<LuaSystemState> _current;

	Callback<> _cbStart;
	Callback<> _cbStop;

public:
	LuaSystem()
	{
		_current = std::make_unique<LuaSystemIdleState>();

		_cbStart = Callback<>([&] { OnStart(); });
		_cbStart = Callback<>([&] { OnStop(); });

		engine::Engine::Instance().OnStartPlaying += _cbStart;
	}

	void OnStart() override
	{
		_current = std::make_unique<LuaSystemPlayState>();
		_current->OnStart();
	}

	void OnUpdate(float deltaTime) override
	{
		_current->OnUpdate(deltaTime);
	}

	void OnStop() override
	{
		_current->OnStop();
		_current = std::make_unique<LuaSystemIdleState>();
	}
};
