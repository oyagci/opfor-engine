#pragma once

#include "ecs/System.hpp"
#include "components/LuaScriptComponent.hpp"
#include "Engine.hpp"

//
//   State Transitions:
//
//   +-----------<----------+
//   |                      |
// [Start] --+-> [Play] -> [Stop]
//           |     |
//           +<- [Pause]
//
class LuaSystemState_Start;
class LuaSystemState_Play;
class LuaSystemState_Pause;
class LuaSystemState_Stop;

class LuaSystemState
{
public:
	ecs::EntityManager *EntityMgr;

	using NextState = std::optional<std::unique_ptr<LuaSystemState>>;

protected:
	template <typename T>
	auto CreateState()
	{
		static_assert(std::is_base_of<LuaSystemState, T>::value &&
			!std::is_same<LuaSystemState, T>::value, "T must be derived from LuaSystemState");

		auto state = std::make_unique<T>();
		state->EntityMgr = EntityMgr;

		return state;
	}

public:
	virtual ~LuaSystemState() {}
	virtual NextState OnStart() { return std::nullopt; }
	virtual NextState OnUpdate(float) { return std::nullopt; }
	virtual NextState OnStop() { return std::nullopt; }
};

class LuaSystemState_Start : public LuaSystemState
{
public:
	NextState OnStart() override;
};

class LuaSystemState_Play : public LuaSystemState
{
public:
	NextState OnUpdate(float deltaTime) override
	{
		NextState nextState;

		auto luaScripts = EntityMgr->GetEntities<LuaScriptComponent>();

		for (auto const &script : luaScripts) {
			auto [ lua ] = script->GetAll();

			lua.Runtime.Call("onUpdate", deltaTime);
		}

		return nextState;
	}

	NextState OnStop() override
	{
		NextState nextState;

		auto luaScripts = EntityMgr->GetEntities<LuaScriptComponent>();

		for (auto const &script : luaScripts) {
			auto [ lua ] = script->GetAll();

			lua.Runtime.Call("onStop");
		}

		nextState = CreateState<LuaSystemState_Start>();
		return nextState;
	}
};

inline LuaSystemState::NextState LuaSystemState_Start::OnStart()
{
	NextState nextState;

	auto luaScripts = EntityMgr->GetEntities<LuaScriptComponent>();

	for (auto const &script : luaScripts) {
		auto [ lua ] = script->GetAll();

		lua.Runtime.Call("onStart");
	}

	nextState = CreateState<LuaSystemState_Play>();
	return nextState;
}

class LuaSystem : public ecs::ComponentSystem
{
private:
	std::unique_ptr<LuaSystemState> _current;

	Callback<> _cbStart;
	Callback<> _cbStop;

private:
public:
	LuaSystem()
	{
		_cbStart = Callback<>([&] { OnStart(); });
		_cbStop = Callback<>([&] { OnStop(); });

		opfor::Engine::Instance().OnStartPlaying += _cbStart;
		opfor::Engine::Instance().OnStopPlaying += _cbStop;

		_current = std::make_unique<LuaSystemState_Start>();
	}

	void OnStart() override
	{
		_current->EntityMgr = EntityMgr;
		auto next = _current->OnStart();
		if (next) {
			_current = std::move(next.value());
		}
	}

	void OnUpdate(float deltaTime) override
	{
		_current->EntityMgr = EntityMgr;
		auto next = _current->OnUpdate(deltaTime);
		if (next) {
			_current = std::move(next.value());
		}
	}

	void OnStop() override
	{
		_current->EntityMgr = EntityMgr;
		auto next = _current->OnStop();
		if (next) {
			_current = std::move(next.value());
		}
	}
};
