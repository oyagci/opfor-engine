#include <cstdint>
#include <glm/gtx/matrix_decompose.hpp>
#include "core/Application.hpp"
#include "layers/ImGuiLayer.hpp"
#include "components/SelectedComponent.hpp"
#include "ImGuizmo.h"
#include "EditorViewport.hpp"

void EditorViewport::OnDrawGUI()
{
	ImVec2 viewportSize = ImGuiLayer::Get().GetViewportSize();
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar;

	if (ImGui::Begin("Viewport", nullptr, windowFlags)) {
		viewportSize = ImGui::GetWindowSize();

		const ImVec2 winSize = ImGui::GetWindowSize();

		if (abs(prevViewportSize.x - viewportSize.x) > 0.01 ||
			abs(prevViewportSize.y - viewportSize.y) > 0.01) {

			fmt::print("({}, {}) - ({}, {}) = ({}, {})\n",
				prevViewportSize.x, prevViewportSize.y,
				viewportSize.x, viewportSize.y,
				prevViewportSize.x - viewportSize.x, prevViewportSize.y - viewportSize.y);

			opfor::ViewportResizeEvent e(static_cast<int>(viewportSize.x), static_cast<int>(viewportSize.y));
			opfor::Application::Get().OnEvent(e);
			opfor::Application::Get().GetCameraController().GetCamera().SetAspect(viewportSize.x / viewportSize.y);

			prevViewportSize = viewportSize;
		}

		// float targetAspectRatio = 16.0f / 9.0f;
		const float targetAspectRatio = viewportSize.x / viewportSize.y;

		void *rawHandle = (void*)(intptr_t)opfor::Application::Get().GetViewport()->GetTexture()->GetRawHandle();

		if ((winSize.x / winSize.y) < targetAspectRatio) {
			ImGui::Image(rawHandle,
				{ winSize.x, winSize.x / targetAspectRatio },
				ImVec2(0.0f, 1.0f),
				ImVec2(1.0f, 0.0f),
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
				ImVec4(1.0f, 1.0f, 1.0f, 0.0f));

			viewportSize.y = winSize.x / targetAspectRatio;
		}
		else {
			ImGui::Image(rawHandle,
				{ winSize.y * targetAspectRatio, winSize.y },
				ImVec2(0.0f, 1.0f),
				ImVec2(1.0f, 0.0f),
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
				ImVec4(1.0f, 1.0f, 1.0f, 0.0f));

			viewportSize.x = winSize.y * targetAspectRatio;
		}

		const ImVec2 viewportPosition = ImGui::GetWindowPos();
		DrawGuizmoSelectedEntity(viewportSize, viewportPosition);
	}
	ImGui::End();

	ImGui::PopStyleVar();
}

void EditorViewport::DrawGuizmoSelectedEntity(ImVec2 viewportSize, ImVec2 viewportPosition)
{
	auto &camera = opfor::Application::Get().GetCameraController().GetCamera();
	auto selectedEnt = opfor::Application::Get().GetEntities<TransformComponent, SelectedComponent>();
	
	if (!selectedEnt.empty()) {

		auto &selected = selectedEnt[0]->Get<TransformComponent>();

		glm::mat4 model(1.0f);
		model = glm::translate(model, selected.position);
		model = glm::scale(model, selected.scale);

		bool changed = false;

		ImGuizmo::BeginFrame();
		ImGuizmo::SetRect(viewportPosition.x, viewportPosition.y, viewportSize.x, viewportSize.y);
		{
			std::array<float, 3> rotation{} , translation{}, scale{};
			ImGuizmo::DecomposeMatrixToComponents(&model[0][0],
				translation.data(), rotation.data(), scale.data());

			ImGuizmo::RecomposeMatrixFromComponents(translation.data(), rotation.data(), scale.data(),
				&model[0][0]);
		}

		glm::quat rotation;
		glm::vec3 skew(0.0f);
		glm::vec4 persp(0.0f);
		ImGuizmo::DecomposeMatrixToComponents(&model[0][0], &selected.position[0], &rotation[0], &selected.scale[0]);

		glm::mat4 cpy = model;

		ImGuizmo::SetDrawlist();
		ImGuizmo::Manipulate(&camera.GetViewMatrix()[0][0], &camera.GetProjection()[0][0],
			ImGuizmo::TRANSLATE,
			ImGuizmo::WORLD,
			&model[0][0],
			nullptr,
			nullptr);

		if (cpy != model) {
			changed = true;
		}

		if (changed) {
			glm::decompose(model, selected.scale, rotation, selected.position, skew, persp);
		}

	}
}
