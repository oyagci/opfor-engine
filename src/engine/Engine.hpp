#pragma once

#include <memory>
#include "lazy.hpp"
#include "ui/UI.hpp"
#include "ecs/ecs.hpp"
#include "EngineObjects.hpp"

using namespace lazy;
using namespace graphics;
using namespace inputs;
using namespace utils;

namespace ecs
{
	class SystemManager;
	class EntityManager;
}

namespace engine
{

class Engine
{
private:
	std::unique_ptr<Display> _display;
	std::unique_ptr<Camera> _camera;
	std::unique_ptr<UI> _ui;

	ecs::ECSEngine _ecs;

	ecs::EntityManager *_entityManager;
	ecs::SystemManager *_systemManager;

	std::vector<std::unique_ptr<EngineObject>> _engineObjects;

private:
	Engine();

public:
	Engine(Engine const &) = delete;
	void operator=(Engine const &) = delete;

	static Engine &Instance()
	{
		static Engine engine;
		return engine;
	}

	int Run();
	void Update();
	void UpdateObjects();
	void UpdateSubobjects(std::vector<EngineObject*> subobjects);

	lazy::graphics::Display *GetDisplay() { return _display.get(); }

	template <typename T, typename ... ArgTypes>
	[[nodiscard]] T *CreateEngineObject(ArgTypes... args)
	{
		static_assert(std::is_base_of<EngineObject, T>::value && !std::is_same<EngineObject, T>::value,
			"T must be derived from EngineObject");

		auto object = std::make_unique<T>(&_ecs, std::forward<ArgTypes>(args)...);
		auto ret = object.get();

		_engineObjects.push_back(std::move(object));

		return ret;
	}

	template <typename T, typename ... ArgTypes>
	void CreateComponentSystem(ArgTypes... args)
	{
		static_assert(std::is_base_of<ecs::ComponentSystem, T>::value && !std::is_same<ecs::ComponentSystem, T>::value,
			"T must be derived from EngineObject");

		_ecs.GetSystemManager()->InstantiateSystem<T>(std::forward(args)...);
	}

	template <typename ... ArgTypes>
	[[nodiscard]] ecs::IEntity<ArgTypes...> *CreateEntity()
	{
		return _ecs.GetEntityManager()->CreateEntity<ArgTypes...>();
	}
};

}
