#pragma once
#include "opfor/core/base.hpp"

namespace opfor {

	class ISceneState
	{
	public:
		virtual ~ISceneState() = default;

		virtual Optional<UniquePtr<ISceneState>> Update() { return std::nullopt; }
		virtual Optional<UniquePtr<ISceneState>> Start() { return std::nullopt; }
		virtual Optional<UniquePtr<ISceneState>> Stop() { return std::nullopt; }

	protected:
		template <typename StateType, typename ... StateArgs>
		UniquePtr<ISceneState> MakeState(StateArgs&& ...args)
		{
			static_assert(std::is_base_of<ISceneState, StateType>::value
				&& !std::is_same<ISceneState, StateType>::value,
				"T must be derived from ISceneState");
			return MakeUnique<StateType>(std::forward<StateArgs>(args)...);
		}
	};

	class SceneStopped : public ISceneState
	{
	public:
		SceneStopped() {}
	};

	class ScenePlaying : public ISceneState
	{
		Optional<UniquePtr<ISceneState>> Update() override
		{
			return std::nullopt;
		}

		Optional<UniquePtr<ISceneState>> Stop() override
		{
			return MakeState<SceneStopped>();
		}
	};

	class SceneUpdater
	{
	private:
		UniquePtr<ISceneState> _State;
	
	private:
		inline void UpdateState(Optional<UniquePtr<ISceneState>> nextState)
		{
			if (nextState) {
				_State.swap(*nextState);
			}
		}

	public:
		SceneUpdater()
		{
			_State = MakeUnique<SceneStopped>();
		}

		~SceneUpdater() = default;

		void Update() {
			UpdateState(_State->Update());
		}

		void Start() {
			UpdateState(_State->Start());
		}

		void Stop() {
			UpdateState(_State->Stop());
		}
	};

	class Scene
	{
	private:
	public:
		Scene() {}
		virtual ~Scene() {}

		virtual void Build() {}
		virtual void Destroy() {}
		virtual void Start() {}
		virtual void Update() {}
		virtual void Stop() {}

		template <typename T, typename ... Args>
		inline T *CreateEngineObject(Args&& ...args)
		{
			return opfor::Application::Get().CreateEngineObject<T>(std::forward<Args&&>(args)...);
		}
	};
}