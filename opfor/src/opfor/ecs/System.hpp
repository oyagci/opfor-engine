#pragma once

#include "Entity.hpp"
#include "EntityManager.hpp"
#include <memory>
#include <type_traits>
#include <vector>

#ifndef __unused
#define __unused __attribute__((unused))
#endif

namespace ecs
{

class ISystemBase
{
  public:
    EntityManager *EntityMgr;

  public:
    ISystemBase() = default;

    virtual ~ISystemBase()
    {
    }
    virtual void OnUpdate(float deltaTime) = 0;

    ///
    /// Wrapper to call EntityManager::GetEntities
    ///
    template <typename... Components> std::vector<IEntity<Components...> *> GetEntities()
    {
        auto entities = EntityMgr->GetEntities<Components...>();

        return entities;
    }

    std::vector<IEntityBase *> GetAllEntities()
    {
        return EntityMgr->GetAllEntities();
    }
};

class ComponentSystem : public ISystemBase
{
  public:
    virtual ~ComponentSystem()
    {
    }
    virtual void OnStart()
    {
    }
    virtual void OnUpdate(float deltaTime)
    {
    }
    virtual void OnStop()
    {
    }
};

} // namespace ecs
