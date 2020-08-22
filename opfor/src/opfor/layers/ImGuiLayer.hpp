#pragma once

#include "Layer.hpp"
#include "imgui.h"
#include "ecs/Entity.hpp"

class ImGuiLayer : public opfor::Layer
{
private:
	ImVec2 _ViewportPosition;
	ImVec2 _ViewportSize;

	bool _logAutoScroll;
	size_t _SelectedItem = 0;

	ecs::IEntityBase *_currentEntity = nullptr;

	void BeginFrame();
	void EndFrame();

	void DrawGuizmoSelectedEnt();
	void BeginDockspace();
	void EndDockspace();
	void MenuBar();
	void Materials();
	void Log();
	void Properties();
	void PlayMenu();
	void Viewport();
	void SceneHierarchy();

	void ObjectTransform();
	void ObjectMesh();
	void ObjectLight();

	void SetupImGuiStyle();

public:
	ImGuiLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnImGuiRender() override;
};
