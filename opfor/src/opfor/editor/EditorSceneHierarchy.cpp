#include "EditorSceneHierarchy.hpp"
#include "imgui.h"
#include "core/Application.hpp"
#include "fmt/format.h"
#include "layers/ImGuiLayer.hpp"

void EditorSceneHierarchy::OnDrawGUI()
{
	ImGui::Begin("Scene Hierarchy");

	if (ImGui::BeginPopupContextWindow("hierarchy_popup_menu")) {
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("hierarchy_popup_menu")) {
		if (ImGui::BeginMenu("Add...")) {
			if (ImGui::MenuItem("Entity")) {
				//auto ent = opfor::Application::Get().GetCurrentLevel()->CreateEntity();
				//(void)ent;
			}
			if (ImGui::MenuItem("Point Light")) {
				//auto ent = opfor::Application::Get().GetCurrentLevel()->CreateEntity();
				//ent->AddComponents<PointLightComponent>();
				//(void)ent;
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	auto allEnts = opfor::Application::Get().GetAllEntities();

	size_t itemIndex = 0;
	for (auto const &ent : allEnts) {
		auto name = ent->GetName();
		if (ImGui::TreeNodeEx(reinterpret_cast<void*>(itemIndex), static_cast<ImGuiTreeNodeFlags>(0), fmt::format("{}", name).c_str())) {
			ImGui::TreePop();
		}
		if (ImGui::IsItemClicked()) {
			_selectedItem = itemIndex;
			ImGuiLayer::Get().SetSelectedEntity(allEnts[itemIndex]);
			opfor::Application::Get().OnSelectItem(_selectedItem);
		}
		itemIndex++;
	}

	ImGui::End();
}
