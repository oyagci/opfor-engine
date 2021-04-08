#pragma once

#include "Component.hpp"
#include "Entity.hpp"
#include <algorithm>
#include <map>
#include <memory>
#include <opfor/core/base.hpp>
#include <type_traits>
#include <vector>

namespace opfor
{

namespace ecs
{

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
    opfor::Vector<opfor::SharedPtr<IComponentBase>> Components;
    opfor::UnorderedMap<uuids::uuid, opfor::SharedPtr<IEntityBase>> Entities;

    EntityManager_Impl() = default;

    /* EntityManager should be unique */
    EntityManager_Impl(EntityManager_Impl const &) = delete;
    void operator=(EntityManager_Impl const &) = delete;

    template <typename T, typename... Types> IEntity<T, Types...> *CreateEntity()
    {
        static_assert(std::is_base_of<IComponentBase, T>::value, "T must be derived from IComponentBase");

        auto entity = opfor::MakeShared<IEntity<T, Types...>>();

        Entities[entity->GetUuid()] = entity;

        return entity.get();
    }

    ///
    /// Get a vector of non-owned pointers matching the specified list of components
    /// there should be in each element
    ///
    template <typename... Types> opfor::Vector<IEntity<Types...> *> GetEntities()
    {
        opfor::Vector<IEntity<Types...> *> entities;

        // Extract matching entities and put them in the vector
        for (auto &[uuid, e] : Entities)
        {
            if (e->HasComponents<Types...>())
            {
                // We can reinterpret_cast the pointer to any IEntity<...> because
                // the type information is only relevant on the object's construction
                // so there should be no problem as long as the components are present
                auto ptr = reinterpret_cast<IEntity<Types...> *>(e.get());
                entities.push_back(ptr);
            }
        }

        return entities;
    }

    opfor::Vector<IEntityBase *> GetAllEntities()
    {
        opfor::Vector<IEntityBase *> entities;

        for (auto &[uuid, e] : Entities)
        {
            entities.push_back(e.get());
        }

        return entities;
    }

    opfor::Optional<IEntityBase *> GetEntity(uuids::uuid const &uuid)
    {
        auto const &entity = Entities.find(uuid);

        if (entity != Entities.end())
        {
            return entity->second.get();
        }

        return std::nullopt;
    }

    void DeleteEntity(uuids::uuid const &uuid)
    {
        const auto entity = Entities.find(uuid);

        if (entity != Entities.end())
        {
            Entities.erase(entity);
        }
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

    /* An EntityManager should be unique */
    EntityManager(EntityManager const &) = delete;
    void operator=(EntityManager const &) = delete;

    ///
    /// Create an entity with the components given in parameter
    ///
    template <typename... Types> IEntity<Types...> *CreateEntity()
    {
        return Manager.CreateEntity<Types...>();
    }

    ///
    /// Get a vector of entities that contains the list of components given in parameter
    ///
    template <typename... Types> opfor::Vector<IEntity<Types...> *> GetEntities()
    {
        return Manager.GetEntities<Types...>();
    }

    opfor::Vector<IEntityBase *> GetAllEntities()
    {
        return Manager.GetAllEntities();
    }

    opfor::Optional<IEntityBase *> GetEntity(uuids::uuid const &uuid)
    {
        return Manager.GetEntity(uuid);
    }

    void DeleteEntity(uuids::uuid const &uuid)
    {
        Manager.DeleteEntity(uuid);
    }
};

} // namespace ecs

} // namespace opfor
