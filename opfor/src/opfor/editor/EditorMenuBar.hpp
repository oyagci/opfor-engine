#pragma once
#include "EditorWindow.hpp"
#include "imgui.h"

namespace opfor
{

class EditorMenuBar : public IEditorWindow
{
  public:
    void OnDrawGUI() override;
};

} // namespace opfor