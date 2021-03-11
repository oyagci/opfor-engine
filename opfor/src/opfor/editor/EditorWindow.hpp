#pragma once

class IEditorWindow
{
  public:
    virtual ~IEditorWindow()
    {
    }

    virtual void OnDrawGUI() = 0;
};
