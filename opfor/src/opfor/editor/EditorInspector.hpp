#pragma once
#include "EditorWindow.hpp"

namespace ecs
{
class IEntityBase;
}

class EditorInspector : public IEditorWindow
{
  private:
    void ObjectTransform(ecs::IEntityBase *);
    void ObjectMesh(ecs::IEntityBase *);
    void ObjectLight(ecs::IEntityBase *);

  public:
    void OnDrawGUI() override;
};
