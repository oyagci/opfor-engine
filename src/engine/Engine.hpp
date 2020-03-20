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

	template <typename T, typename ... ArgTypes>
	[[nodiscard]] T *CreateEngineObject(ArgTypes... args)
	{
		auto object = std::make_unique<T>(std::forward<ArgTypes>(args)...);
		auto ret = object.get();

		_engineObjects.push_back(std::move(object));

		return ret;
	}
};

}
