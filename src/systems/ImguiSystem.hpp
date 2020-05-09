#pragma once

#include "ecs/System.hpp"
#include "lazy.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "components/DisplayComponent.hpp"
#include "components/PlayerCameraComponent.hpp"

class ImguiSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Display *_display;
	bool _showDemoWindow;

public:
	ImguiSystem() : _display(nullptr), _showDemoWindow(true)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}

	~ImguiSystem()
	{
		ImGui::DestroyContext();
	}

	void OnUpdate(float __unused deltaTime) override
	{
		if (!_display) {
			auto displays = GetEntities<DisplayComponent>();
			_display = displays[0]->GetComponentData<DisplayComponent>().display;
			ImGui_ImplGlfw_InitForOpenGL(_display->getWindow(), true);
			ImGui_ImplOpenGL3_Init("#version 450");
		}

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		ImGui::ShowDemoWindow(&_showDemoWindow);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		auto &kbd = lazy::inputs::input::getKeyboard();

		if (kbd.getKeyDown(GLFW_KEY_ESCAPE)) {
			auto playerEnts = GetEntities<PlayerCameraComponent>();
			if (playerEnts.size() > 0) {
				auto camera = playerEnts[0]->GetComponentData<PlayerCameraComponent>();
				auto displayEnt = GetEntities<DisplayComponent>()[0];
				auto display = displayEnt->GetComponentData<DisplayComponent>();

				// Enable/Disable input for camera
				camera.useInput = !camera.useInput;
				playerEnts[0]->SetComponentData<PlayerCameraComponent>(camera);

				// Enable/Switch cursor
				display.display->showCursor(!camera.useInput);
				displayEnt->SetComponentData<DisplayComponent>(display);
			}
		}
	}
};
