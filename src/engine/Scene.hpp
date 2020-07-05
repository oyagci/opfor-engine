#pragma once

#include <vector>
#include "ecs/ecs.hpp"
#include "uuid.h"
#include <fmt/format.h>
#include <cassert>

#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"

#include "systems/CameraMovementSystem.hpp"

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

	bool _IsPlaying;

	ecs::ECSEngine::Instance *_EditorEcs;
	ecs::IEntityBase *_EditorCamera;

public:
	Scene() : State(std::make_unique<SceneState_Stopped>()), EcsInstance(ecs::ECSEngine::Get().CreateInstance()), _IsPlaying(false)
	{
		assert(EcsInstance);
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
		_IsPlaying = true;
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
		if (!_IsPlaying) { return; }

		State->OnUpdate(deltaTime);
		EcsInstance->SystemManager->Update(deltaTime);

		Update(deltaTime);
	}

	void OnSetup()
	{
		Setup();
	}

	void OnEditorStart()
	{
		_EditorEcs = ecs::ECSEngine::Get().CreateInstance();

		_EditorEcs->SystemManager->InstantiateSystem<CameraMovementSystem>();
		_EditorEcs->EntityManager->CreateEntity<PlayerCameraComponent, TransformComponent>();

		_EditorCamera = _EditorEcs->EntityManager->CreateEntity<PlayerCameraComponent, TransformComponent>();
		auto &camera = _EditorCamera->Get<PlayerCameraComponent>();
			camera.projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
			camera.model = glm::mat4(1.0f);
			camera.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			camera.viewProjection = camera.projection * camera.view;
			camera.exposure = 2.0f;
		_EditorCamera->Get<TransformComponent>().position = { 200.0f, 200.0f, 200.0f };
		_EditorCamera->SetName("Editor Camera");
	}

	void OnEditorUpdate(float deltaTime)
	{
		//_EditorEcs->SystemManager->Update(deltaTime);
	}

	void OnEditorStop()
	{
		_EditorEcs->EntityManager->DeleteEntity(_EditorCamera->GetId());
		_EditorEcs->SystemManager->DeleteSystem<CameraMovementSystem>();
		// TODO: Delete _EditorEcs
	}

	ecs::ECSEngine::Instance &ECS() { return *EcsInstance; }

	/// Setup the base state of the scene.
	/// This is where you should instantiate systems and entities
	virtual void Setup() {}

	/// Called right before the scene starts playing.
	/// This is best suited to attach scripts to your entities.
	virtual void Play() {}

	/// Called every frame while playing.
	/// This is where you should update the logic of your scene
	virtual void Update(float /* deltaTime */) {};
};
