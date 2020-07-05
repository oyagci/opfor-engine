#pragma once

#include <memory>
#include <vector>
#include <type_traits>
#include "System.hpp"
#include "Logger.hpp"

namespace ecs {

class ECSEngine;

class SystemManager_Impl
{
	friend class SystemManager;

private:
	SystemManager_Impl(EntityManager *mgr) : Systems{}, EntityMgr(mgr) {}

	std::unordered_map<TypeIndex, std::unique_ptr<ISystemBase>> Systems;
	EntityManager *EntityMgr;

	template <typename T>
	void InstantiateSystem()
	{
		static_assert(std::is_base_of<ISystemBase, T>::value &&
					 !std::is_same<ISystemBase, T>::value,
					  "T must be derived from ISystemBase");

		if (Systems.find(GetTypeIndex<T>()) != Systems.end()) {
			Logger::Warn("System already exists (Name: {})\n", typeid(T).name());
			return ;
		}

		auto newSystem = std::make_unique<T>();
		newSystem->EntityMgr = EntityMgr;

		Systems[GetTypeIndex<T>()] = std::move(newSystem);
	}

	template <typename T>
	void DeleteSystem()
	{
		static_assert(std::is_base_of<ISystemBase, T>::value &&
					 !std::is_same<ISystemBase, T>::value,
					  "T must be derived from ISystemBase");

		auto system = Systems.find(GetTypeIndex<T>());

		if (system != Systems.end()) {
			Systems.erase(system);
		}
	}

	void Update(float deltaTime)
	{
		for (auto &s : Systems) {
			s.second->OnUpdate(deltaTime);
		}
	}
};

class SystemManager
{
private:
	SystemManager_Impl Manager;

public:
	SystemManager(EntityManager *entityMgr) : Manager(entityMgr) {}

	SystemManager(SystemManager const &) = delete;
	void operator=(SystemManager const &) = delete;

	template <typename T>
	void InstantiateSystem()
	{
		Manager.InstantiateSystem<T>();
	}

	template <typename T>
	void DeleteSystem()
	{
		Manager.DeleteSystem<T>();
	}

	void Update(float deltaTime)
	{
		Manager.Update(deltaTime);
	}
};

}
