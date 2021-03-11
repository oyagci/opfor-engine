#pragma once
#include "EditorWindow.hpp"
#include "imgui.h"

class EditorViewport final : public IEditorWindow
{
  private:
    void DrawGuizmoSelectedEntity(ImVec2 viewportSize, ImVec2 viewportPosition);

  public:
    void OnDrawGUI() override;
};
