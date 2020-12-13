#pragma once

#include "Layer.hpp"
#include "imgui.h"
#include "ecs/Entity.hpp"
#include <glm/vec2.hpp>
#include "editor/EditorViewport.hpp"
#include "editor/EditorMenuBar.hpp"

class ImGuiLayer : public opfor::Layer
{
private:
	ImVec2 _ViewportPosition;
	ImVec2 _ViewportSize;

	bool _logAutoScroll = true;
	size_t _SelectedItem = 0;

	ecs::IEntityBase *_currentEntity = nullptr;

	opfor::UniquePtr<EditorViewport> _viewport;
	opfor::UniquePtr<EditorMenuBar> _menuBar;

	void BeginFrame();
	void EndFrame();

	void DrawGuizmoSelectedEnt();
	void BeginDockspace();
	void EndDockspace();
	void MenuBar();
	void Materials();
	void Log();
	void Properties();
	void Viewport();
	void SceneHierarchy();

	void ObjectTransform();
	void ObjectMesh();
	void ObjectLight();

	void SetupImGuiStyle();

private:
	ImGuiLayer();

public:
	static ImGuiLayer &Get()
	{
		static ImGuiLayer instance;
		return instance;
	}

	void OnAttach() override;
	void OnDetach() override;
	void OnImGuiRender() override;

	ImVec2 GetViewportSize() const { return _ViewportSize; }
	ImVec2 GetViewportPosition() const { return _ViewportPosition; }
};
