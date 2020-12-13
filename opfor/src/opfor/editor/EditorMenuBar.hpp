#pragma once
#include "imgui.h"
#include "EditorWindow.hpp"

class EditorMenuBar : public IEditorWindow
{
public:
	void OnDrawGUI() override;
};