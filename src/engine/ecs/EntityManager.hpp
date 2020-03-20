#pragma once

#include <memory>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <map>
#include "Component.hpp"
#include "Entity.hpp"

namespace ecs {

/*
 * Keeps track of every IComponentBase
 */
class EntityManager_Impl
{
	/*
	 * Can/Should only be accessed by EntityManager
	 */
	friend class EntityManager;

private:
	/* Container of managed components */
	std::vector<std::shared_ptr<IComponentBase>> Components;
	std::map<TypeIndex, std::shared_ptr<IEntityBase>> Entities;

	EntityManager_Impl()
	{
	};

	/* EntityManager's should be unique */
	EntityManager_Impl(EntityManager_Impl const &) = delete;
	void operator=(EntityManager_Impl const &) = delete;

	template <typename T, typename ... Types>
	IEntity<T, Types...> *CreateEntity()
	{
		static_assert(std::is_base_of<IComponentBase, T>::value,
			"T must be derived from IComponentBase");

		auto entity = std::make_shared<IEntity<T, Types...>>();

		Entities[GetTypeIndex<IEntity<T, Types...>>()] = entity;

		return entity.get();
	}

	template <typename... Types>
	std::vector<IEntity<Types...>*> GetEntities()
	{
		std::vector<IEntity<Types...>*> entities;

		for (auto &pair : Entities) {
			auto e = pair.second.get();
			if (e->HasComponents<Types...>()) {
				entities.push_back(dynamic_cast<IEntity<Types...>*>(e));
			}
		}

		return entities;
	}
};

class EntityManager
{
private:
	EntityManager_Impl Manager;
public:
	EntityManager()
	{
	}

	/* An EntityManager's should be unique */
	EntityManager(EntityManager const &) = delete;
	void operator=(EntityManager const &) = delete;

	template <typename ... Types>
	IEntity<Types...> *CreateEntity()
	{
		return Manager.CreateEntity<Types...>();
	}

	template <typename ... Types>
	std::vector<IEntity<Types...>*> GetEntities()
	{
		return Manager.GetEntities<Types...>();
	}
};

}
