#pragma once
#include "EditorWindow.hpp"
#include "imgui.h"

class EditorMenuBar : public IEditorWindow
{
  public:
    void OnDrawGUI() override;
};