#pragma once
#include "EditorWindow.hpp"
#include <opfor/core/base.hpp>
#include <opfor/ecs/Component.hpp>

namespace opfor
{

class EditorInspector : public IEditorWindow
{
  private:
    static opfor::String FormatName(opfor::String original);
    static void DrawComponentControls(ecs::IComponentBase &component);

  public:
    void OnDrawGUI() override;
    static void DrawDefaultControl(opfor::String const &name, ecs::IComponentBase &component, rfk::Field const &field);
};

} // namespace opfor
