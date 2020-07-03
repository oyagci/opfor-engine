#pragma once

#include <vector>
#include "ecs/ecs.hpp"
#include "uuid.h"
#include <fmt/format.h>
#include <cassert>

// Scene State
// =============================================================================
// Describes the different state transitions the state machine has
class SceneState
{
	friend class Scene;

protected:
	using NextState = std::optional<std::unique_ptr<SceneState>>;

	std::vector<std::unique_ptr<ecs::IEntityBase>> Entities;

	template <typename T>
	auto CreateState()
	{
		auto newState = std::make_unique<T>();

		return newState;
	}

	ecs::ECSEngine::Instance *EcsInstance;

public:
	virtual ~SceneState() {}

	virtual NextState OnPlay() { return std::nullopt; };
	virtual NextState OnStop() { return std::nullopt; };

	virtual void OnUpdate(float) {}
};


// Scene State : Playing
// =============================================================================
class SceneState_Playing : public SceneState
{
public:
	NextState OnStop() override;
	void OnUpdate(float deltaTime) override
	{
	}
};


// Scene State: Stopped
// =============================================================================
class SceneState_Stopped : public SceneState
{
public:
	NextState OnPlay() override;
};


// Inline Definitions of state functions
// =============================================================================
inline auto SceneState_Playing::OnStop() -> NextState
{
	// TODO: Do the transition...
	return std::make_optional(CreateState<SceneState_Stopped>());
}

inline auto SceneState_Stopped::OnPlay() -> NextState
{
	// TODO: Do the transition...
	// Create the level's entities.
	return std::make_optional(CreateState<SceneState_Playing>());
}


// Scene
// =============================================================================
class Scene
{
private:
	std::unique_ptr<SceneState> State;
	ecs::ECSEngine::Instance *EcsInstance;

public:
	Scene() : State(std::make_unique<SceneState_Stopped>()), EcsInstance(ecs::ECSEngine::Get().CreateInstance())
	{
	}

	virtual ~Scene()
	{
		ecs::ECSEngine::Get().RemoveInstance(EcsInstance->Uuid);
	}

	void OnPlay()
	{
		auto next = State->OnPlay();
		if (next.has_value()) {
			assert(next.value().get());
			State = std::move(next.value());
			State->EcsInstance = EcsInstance;
		}

		Play();
	}

	void OnStop()
	{
		auto next = State->OnStop();
		if (next.has_value()) {
			State = std::move(next.value());
		}
	}

	void OnUpdate(float deltaTime)
	{
		State->OnUpdate(deltaTime);
		EcsInstance->SystemManager->Update(deltaTime);

		Update(deltaTime);
	}

	ecs::ECSEngine::Instance &ECS() { return *EcsInstance; }

	virtual void Play() {};
	virtual void Update(float /* deltaTime */) {};
};
