#pragma once

#include "ecs/System.hpp"
#include "lazy.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "components/DisplayComponent.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "ImGuizmo/ImGuizmo.h"
#include <glm/gtx/matrix_decompose.hpp>

class ImguiSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Display *_display;

public:
	ImguiSystem() : _display(nullptr)
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
			_display = displays[0]->Get<DisplayComponent>().display;
			ImGui_ImplGlfw_InitForOpenGL(_display->getWindow(), true);
			ImGui_ImplOpenGL3_Init("#version 450");
		}

		auto cameraEnt = GetEntities<PlayerCameraComponent>()[0];
		auto camera = cameraEnt->Get<PlayerCameraComponent>();

		glm::mat4 guizmo(1.0f);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		auto selectedEnt = GetEntities<TransformComponent, SelectedComponent>();
		if (selectedEnt.size() > 0) {

			auto selected = selectedEnt[0]->Get<TransformComponent>();

			glm::mat4 model(1.0f);
			model = glm::scale(model, selected.scale);
			model = glm::translate(model, selected.position);

			ImGuizmo::BeginFrame();
			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
			ImGuizmo::Manipulate(&camera.view[0][0], &camera.projection[0][0],
				ImGuizmo::TRANSLATE,
				ImGuizmo::WORLD,
				&model[0][0],
				nullptr,
				nullptr);

			{
				std::array<float, 3> rotation{} , translation{}, scale{};
				ImGuizmo::DecomposeMatrixToComponents(&model[0][0],
					translation.data(), rotation.data(), scale.data());

				ImGui::Begin("Object Transform");
				ImGui::InputFloat3("Position", translation.data(), 3);
				ImGui::InputFloat3("Rotation", rotation.data(), 3);
				ImGui::InputFloat3("Scale", scale.data(), 3);
				if (ImGui::Button("Reset")) {
					rotation.fill(0.0f);
					translation.fill(0.0f);
					scale.fill(1.0f);
				}
				ImGui::End();

				ImGuizmo::RecomposeMatrixFromComponents(translation.data(), rotation.data(), scale.data(),
					&model[0][0]);
			}

			glm::quat rotation;
			glm::vec3 skew(0.0f);
			glm::vec4 persp(0.0f);
//			ImGuizmo::DecomposeMatrixToComponents(&model[0][0], &selected.position[0], &rotation[0], &selected.scale[0]);

			glm::decompose(model, selected.scale, rotation, selected.position, skew, persp);
			selectedEnt[0]->Set<TransformComponent>(selected);

		}

		LightProperties();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		auto &kbd = lazy::inputs::input::getKeyboard();

		if (kbd.getKeyDown(GLFW_KEY_ESCAPE)) {
			auto playerEnts = GetEntities<PlayerCameraComponent>();
			if (playerEnts.size() > 0) {
				auto camera = playerEnts[0]->Get<PlayerCameraComponent>();
				auto displayEnt = GetEntities<DisplayComponent>()[0];
				auto display = displayEnt->Get<DisplayComponent>();

				// Enable/Disable input for camera
				camera.useInput = !camera.useInput;
				playerEnts[0]->Set(camera);

				// Enable/Switch cursor
				display.display->showCursor(!camera.useInput);
				displayEnt->Set(display);
			}
		}
	}

	void LightProperties()
	{
		auto lights = GetEntities<PointLightComponent, TransformComponent>();

		if (lights.size() == 0) { return ; }

		auto [ light, transform ] = lights[0]->GetAll();

		PointLightComponent newLight(light);

		ImGui::Begin("Light Properties");
		ImGui::ColorEdit3("Color", &newLight.Color[0]);
		ImGui::InputFloat("Constant", &newLight.constant);
		ImGui::InputFloat("Linear", &newLight.linear);
		ImGui::InputFloat("Quadratic", &newLight.quadratic);
		ImGui::End();

		lights[0]->Set(newLight);

	}

//	void EditTransform(const glm::mat4 &view, const glm::mat4 &proj)
//	{
//		glm::mat4 matrix(1.0f);
//		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
//		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
//		if (ImGui::IsKeyPressed('t'))
//			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
//		if (ImGui::IsKeyPressed('e'))
//			mCurrentGizmoOperation = ImGuizmo::ROTATE;
//		if (ImGui::IsKeyPressed('r')) // r Key
//			mCurrentGizmoOperation = ImGuizmo::SCALE;
//		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
//			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
//		ImGui::SameLine();
//		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
//			mCurrentGizmoOperation = ImGuizmo::ROTATE;
//		ImGui::SameLine();
//		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
//			mCurrentGizmoOperation = ImGuizmo::SCALE;
//
//		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
//		{
//			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
//				mCurrentGizmoMode = ImGuizmo::LOCAL;
//			ImGui::SameLine();
//			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
//				mCurrentGizmoMode = ImGuizmo::WORLD;
//		}
//		static bool useSnap(false);
//		if (ImGui::IsKeyPressed(83))
//			useSnap = !useSnap;
//		ImGui::Checkbox("", &useSnap);
//		ImGui::SameLine();
//		ImGuiIO& io = ImGui::GetIO();
//		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
//		ImGuizmo::Manipulate(&view[0][0], &proj[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &_guizmo[0][0], &_guizmo[0][0], nullptr);
//	}
};
