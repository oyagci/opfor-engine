#pragma once

#include "Component.hpp"
#include "Entity.hpp"
#include <algorithm>
#include <map>
#include <memory>
#include <type_traits>
#include <vector>

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
    std::vector<std::shared_ptr<IComponentBase>> Components;
    std::vector<std::shared_ptr<IEntityBase>> Entities;

    EntityManager_Impl(){};

    /* EntityManager should be unique */
    EntityManager_Impl(EntityManager_Impl const &) = delete;
    void operator=(EntityManager_Impl const &) = delete;

    template <typename T, typename... Types> IEntity<T, Types...> *CreateEntity()
    {
        static_assert(std::is_base_of<IComponentBase, T>::value, "T must be derived from IComponentBase");

        auto entity = std::make_shared<IEntity<T, Types...>>();

        Entities.push_back(entity);

        return entity.get();
    }

    ///
    /// Get a vector of non-owned pointers matching the specified list of components
    /// there should be in each element
    ///
    template <typename... Types> std::vector<IEntity<Types...> *> GetEntities()
    {
        std::vector<IEntity<Types...> *> entities;

        // Extract matching entities and put them in the vector
        for (auto &e : Entities)
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

    std::vector<IEntityBase *> GetAllEntities()
    {
        std::vector<IEntityBase *> entities;

        for (auto &e : Entities)
        {
            entities.push_back(e.get());
        }

        return entities;
    }

    std::optional<IEntityBase *> GetEntity(size_t id)
    {
        for (auto &ent : Entities)
        {
            if (ent->GetId() == id)
            {
                return std::make_optional(ent.get());
            }
        }

        return std::nullopt;
    }

    void DeleteEntity(unsigned int entityId)
    {
        auto entity = std::find_if(Entities.begin(), Entities.end(),
                                   [entityId](auto const &ptr) { return ptr->GetId() == entityId; });

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
    template <typename... Types> std::vector<IEntity<Types...> *> GetEntities()
    {
        return Manager.GetEntities<Types...>();
    }

    std::vector<IEntityBase *> GetAllEntities()
    {
        return Manager.GetAllEntities();
    }

    std::optional<IEntityBase *> GetEntity(size_t id)
    {
        return Manager.GetEntity(id);
    }

    void DeleteEntity(unsigned int entityId)
    {
        Manager.DeleteEntity(entityId);
    }
};

} // namespace ecs
