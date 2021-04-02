#pragma once
#include "opfor/core/base.hpp"
#include "opfor/ecs/Entity.hpp"
#include <uuid.h>

namespace opfor
{

class Editor
{
  private:
    inline static Vector<ecs::IEntityBase *> _selectedEntities;

  public:
    static void SelectEntity(uuids::uuid const &uuid);

    static Vector<ecs::IEntityBase *> const &Selection();
};

} // namespace opfor
