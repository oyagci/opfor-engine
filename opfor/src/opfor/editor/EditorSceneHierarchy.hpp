#pragma once
#include "EditorWindow.hpp"

class EditorSceneHierarchy : public IEditorWindow
{
  private:
    int _selectedItem = 0;

  public:
    void OnDrawGUI() override;
};
