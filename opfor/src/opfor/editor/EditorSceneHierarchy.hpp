#pragma once
#include "EditorWindow.hpp"
#include "opfor/core/base.hpp"
#include "uuid.h"
#include <opfor/ecs/ecs.hpp>

class EditorSceneHierarchy : public IEditorWindow
{
  private:
    struct EntityHierarchy
    {
        opfor::String name;
        uuids::uuid uuid;
        ecs::IEntityBase const *entity = nullptr;
        bool selected = false;
        opfor::Vector<EntityHierarchy *> children;
    };

  private:
    int _selectedItem = 0;

    void DrawHierarchy(EntityHierarchy const &hierarchy, size_t idx) const;

  public:
    void OnDrawGUI() override;
};
