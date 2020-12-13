#pragma once
#include "imgui.h"
#include "EditorWindow.hpp"

class EditorViewport final : public IEditorWindow
{
private:
	void DrawGuizmoSelectedEntity(ImVec2 viewportSize, ImVec2 viewportPosition);
public:
	void OnDrawGUI() override;
};
