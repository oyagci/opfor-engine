#pragma once

#include <memory>
#include <vector>
#include <type_traits>
#include "System.hpp"

namespace ecs {

class ECSEngine;

class SystemManager_Impl
{
	friend class SystemManager;

private:
	SystemManager_Impl() = default;

	std::vector<std::unique_ptr<ISystemBase>> Systems;
	EntityManager *EntityMgr;

	template <typename T>
	void InstantiateSystem()
	{
		static_assert(std::is_base_of<ISystemBase, T>::value &&
					 !std::is_same<ISystemBase, T>::value,
					  "T must be derived from ISystemBase");

		auto newSystem = std::make_unique<T>();
		newSystem->EntityMgr = EntityMgr;

		Systems.push_back(std::move(newSystem));
	}

	void Update(float deltaTime)
	{
		for (auto &s : Systems) {
			s->OnUpdate(deltaTime);
		}
	}
};

class SystemManager
{
private:
	SystemManager_Impl Manager;

public:
	SystemManager(EntityManager *entityMgr)
	{
		Manager.EntityMgr = entityMgr;
	}

	SystemManager(SystemManager const &) = delete;
	void operator=(SystemManager const &) = delete;

	template <typename T>
	void InstantiateSystem()
	{
		Manager.InstantiateSystem<T>();
	}

	void Update(float deltaTime)
	{
		Manager.Update(deltaTime);
	}
};

}
