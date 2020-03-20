#pragma once

#include <type_traits>
#include <memory>
#include <vector>
#include "Entity.hpp"
#include "EntityManager.hpp"

#ifndef __unused
# define __unused __attribute__((unused))
#endif

namespace ecs
{

class ISystemBase
{
public:
	EntityManager *EntityMgr;

public:
	ISystemBase() = default;

	virtual ~ISystemBase() {}
	virtual void OnUpdate(float deltaTime) = 0;

	template <typename ... Components>
	std::vector<IEntity<Components...>*> GetEntities()
	{
		auto entities = EntityMgr->GetEntities<Components...>();

		return entities;
	}
};

class ComponentSystem : public ISystemBase
{
public:
	virtual ~ComponentSystem() {}
	virtual void OnUpdate(float __unused deltaTime)
	{
	}
};

}
