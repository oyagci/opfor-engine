#pragma once

#include <glm/gtx/matrix_decompose.hpp>
#include <unistd.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "ImGuizmo/ImGuizmo.h"

#include "ecs/System.hpp"

#include "components/DisplayComponent.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/LuaScriptComponent.hpp"

#include "lazy.hpp"
#include "Engine.hpp"
#include "Logger.hpp"
#include "ShaderManager.hpp"
#include "nfd.hpp"

class ImguiSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Display *_display;

	bool _logAutoScroll;
	ecs::IEntityBase *_currentEntity;

private:
	void BeginFrame()
	{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
	}

	void EndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void DrawGuizmoSelectedEnt()
	{
		auto cameraEnt = GetEntities<PlayerCameraComponent>()[0];
		auto camera = cameraEnt->Get<PlayerCameraComponent>();
		auto selectedEnt = GetEntities<TransformComponent, SelectedComponent>();
		if (selectedEnt.size() > 0) {

			auto selected = selectedEnt[0]->Get<TransformComponent>();

			glm::mat4 model(1.0f);
			model = glm::translate(model, selected.position);
			model = glm::scale(model, selected.scale);

			bool changed = false;

			ImGuizmo::BeginFrame();
			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
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

			ImGuizmo::Manipulate(&camera.view[0][0], &camera.projection[0][0],
				ImGuizmo::TRANSLATE,
				ImGuizmo::WORLD,
				&model[0][0],
				nullptr,
				nullptr);

			if (cpy != model) {
				changed = true;
			}

			glm::decompose(model, selected.scale, rotation, selected.position, skew, persp);
			if (changed) {
				selectedEnt[0]->Set(selected);
			}

		}
	}

	void BeginDockspace()
	{
		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
		bool is_open = true;

		ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar |
									   ImGuiWindowFlags_NoDocking |
									   ImGuiWindowFlags_NoTitleBar |
									   ImGuiWindowFlags_NoCollapse |
									   ImGuiWindowFlags_NoResize |
									   ImGuiWindowFlags_NoMove |
									   ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Dockspace", &is_open, windowFlags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		ImGuiIO &io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
		}

	}

	void EndDockspace()
	{
		ImGui::End();
	}

	void MenuBar()
	{
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Close")) {
					opfor::Engine::Instance().Close();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Level")) {
				}
				if (ImGui::MenuItem("Open Level")) {
					char *outPath = nullptr;
					NFD_OpenDialog(nullptr, getcwd(nullptr, 0), &outPath);
					if (outPath) {
						opfor::Engine::Instance().LoadLevel(outPath);
					}
				}
				if (ImGui::MenuItem("Save Level")) {
				}
				if (ImGui::MenuItem("Save Level As...")) {
					char *outPath = nullptr;
					NFD_OpenDialog(nullptr, getcwd(nullptr, 0), &outPath);
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Lighting")) {
				if (ImGui::MenuItem("Build")) {
					/* Build Lighting for the Level */
					opfor::Engine::Instance().OnBuildLighting();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void Materials()
	{
		auto materialList = opfor::Engine::Instance().GetMaterialList();
		std::sort(materialList.begin(), materialList.end());
		ImGui::Begin("Materials");
		if (ImGui::TreeNode("Materials")) {
			for (auto const &m: materialList) {
				ImGui::Text("%s", m.c_str());
			}
			ImGui::TreePop();
		}
		ImGui::End();
	}

	void Log()
	{
		if (!ImGui::Begin("Log")) {
			ImGui::End();
			return ;
		}

		if (ImGui::BeginPopup("Options")) {
			ImGui::Checkbox("Auto-Scroll", &_logAutoScroll);
			ImGui::EndPopup();
		}

		if (ImGui::Button("Options")) {
			ImGui::OpenPopup("Options");
		}
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		if (clear) {
			Logger::Clear();
		}

		ImGui::Separator();
		ImGui::BeginChild("scolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		auto &logs = Logger::GetLog();
		auto &lineOffsets = Logger::GetLineOffsets();

		const char *bufp = logs.data();
		const char *bufendp = logs.data() + logs.size();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		ImGuiListClipper clipper;
		clipper.Begin(lineOffsets.size());
		while (clipper.Step()) {
			for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
				const char *line_start = bufp + lineOffsets[line_no];
				const char *line_end = (line_no + 1 < static_cast<int>(lineOffsets.size())) ?
					(bufp + lineOffsets[line_no + 1] - 1) : bufendp;
				ImGui::TextUnformatted(line_start, line_end);
			}
		}
		clipper.End();

		ImGui::PopStyleVar();

		if (_logAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
			ImGui::SetScrollHereY(1.0f);
		}

		ImGui::EndChild();

		ImGui::End();
	}

	size_t selectedItem = 0;

	void SceneHierarchy()
	{
		ImGui::Begin("Scene Hierarchy");

		if (ImGui::BeginPopupContextWindow("hierarchy_popup_menu")) {
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("hierarchy_popup_menu")) {
			if (ImGui::BeginMenu("Add...")) {
				if (ImGui::MenuItem("Entity")) {
					auto ent = opfor::Engine::Instance().GetCurrentLevel()->CreateEntity();
					(void)ent;
				}
				if (ImGui::MenuItem("Point Light")) {
					auto ent = opfor::Engine::Instance().GetCurrentLevel()->CreateEntity();
					ent->AddComponents<PointLightComponent>();
					(void)ent;
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}

		auto allEnts = GetAllEntities();

		size_t itemIndex = 0;
		for (auto const &ent : allEnts) {
			auto name = ent->GetName();
			name += "##" + std::to_string(itemIndex);
			if (ImGui::Selectable(name.c_str(), selectedItem == itemIndex)) {
				Logger::Verbose("Clicked on item {} (prev. {}) (ID. {})\n",
					itemIndex, selectedItem, allEnts[itemIndex]->GetId());
				selectedItem = itemIndex;
				_currentEntity = allEnts[itemIndex];
				opfor::Engine::Instance().OnSelectItem(selectedItem);
			}
			itemIndex++;
		}

		ImGui::End();
	}

	void ObjectTransform()
	{
		if (!_currentEntity) { return ; }
		if (!_currentEntity->HasComponents<TransformComponent>()) { return; }

		if (!ImGui::CollapsingHeader("Transform")) { return ; }

		bool changed = false;

		auto &transform = _currentEntity->Get<TransformComponent>();

		std::array<float, 3> rotation{};
		std::array<float, 3> translation{ transform.position.x, transform.position.y, transform.position.z };
		std::array<float, 3> scale{ transform.scale.x, transform.scale.y, transform.scale.z };

		changed += ImGui::InputFloat3("Position", translation.data(), 3);
		changed += ImGui::InputFloat3("Rotation", rotation.data(), 3);
		changed += ImGui::InputFloat3("Scale", scale.data(), 3);

		if (ImGui::Button("Reset")) {
			rotation.fill(0.0f);
			translation.fill(0.0f);
			scale.fill(1.0f);
			changed = true;
		}

		if (changed) {
			transform.position = { translation[0], translation[1], translation[2] };
			transform.scale = { scale[0], scale[1], scale[2] };
		}
	}

	void ObjectMesh()
	{
		if (!_currentEntity) { return ; }
		if (!_currentEntity->HasComponents<ModelComponent>()) { return; }

		if (!ImGui::CollapsingHeader("Static Mesh")) { return ; }

		auto &model = _currentEntity->Get<ModelComponent>();

		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
		ImGui::Columns(2, NULL, true);

		ImGui::Text("File Path");
		ImGui::NextColumn();

		std::string inputFilePath = model.Path;

		if (ImGui::InputText("##FilePath", &inputFilePath)) {
		}
		ImGui::SameLine();
		if (ImGui::Button("...")) {
			char *newPath = nullptr;
			// Open File Dialog
			if (NFD_OpenDialog(nullptr, getcwd(nullptr, 0), &newPath) == NFD_OKAY
				&& newPath) {
				model.Path = std::string(newPath);
				opfor::Engine::Instance().OnRebuildModel(model);
			}
		}

		ImGui::Columns(1);
	}

	void ObjectLight()
	{
		if (!_currentEntity) { return ; }
		if (!_currentEntity->HasComponents<PointLightComponent>()) { return; }

		if (!ImGui::CollapsingHeader("Light")) { return ; }

		auto &light = _currentEntity->Get<PointLightComponent>();

		std::array<const char *, 1> lightTypes = { "Point" };
		size_t currentTypeIdx = 0;
		const char *comboLabel = lightTypes[currentTypeIdx];

		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
		ImGui::Columns(2, NULL, true);

		ImGui::Text("Type");
		ImGui::NextColumn();
		if (ImGui::BeginCombo("##LightTypeCombo", comboLabel, 0)) {
			for (size_t i = 0; i < lightTypes.size(); i++) {
				const bool isSelected = currentTypeIdx == i;
				if (ImGui::Selectable(lightTypes[i], isSelected)) {
					currentTypeIdx = i;
				}

				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::NextColumn();

		ImGui::Text("Color");
		ImGui::NextColumn();
		std::array<float, 3> color{ light.Color[0], light.Color[1], light.Color[2] };
		if (ImGui::ColorEdit3("##LightColor", color.data())) {
			light.Color = { color[0], color[1], color[2] };
		}
		ImGui::NextColumn();

		ImGui::Text("Intensity");
		ImGui::NextColumn();
		float intensity = light.Intensity;
		if (ImGui::InputFloat("##LightIntensity", &intensity)) {
			light.Intensity = intensity;
		}

		ImGui::Columns(1);
	}

	void ObjectLuaScript()
	{
		if (!_currentEntity) { return ; }
		if (!_currentEntity->HasComponents<LuaScriptComponent>()) { return; }

		if (!ImGui::CollapsingHeader("Lua Script")) { return ; }

		auto &luaScript = _currentEntity->Get<LuaScriptComponent>();

		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
		ImGui::Columns(2, NULL, true);

		std::string scriptPath = luaScript.Path;
		ImGui::Text("Script");
		ImGui::NextColumn();
		ImGui::InputText("##ScriptPath", &scriptPath);
		ImGui::SameLine();
		if (ImGui::Button("...##LuaScriptPathButton")) {
			char *newPath = nullptr;
			if (NFD_OpenDialog(nullptr, getcwd(nullptr, 0), &newPath) != NFD_CANCEL &&
				newPath != nullptr) {
				luaScript.Path = std::string(newPath);
				opfor::Engine::Instance().OnReloadScript(luaScript);
				luaScript.Runtime.PushGlobal("__ENTITY_ID__", _currentEntity->GetId());
			}
		}
	}

	void Properties()
	{
		if (_currentEntity == nullptr) { return ; }

		ImGui::Begin("Properties");
			std::string entityName = _currentEntity->GetName();
			if (ImGui::InputText("##EntityName", &entityName)) {
				_currentEntity->SetName(entityName);
			}

			uuids::uuid const &uuid = _currentEntity->GetUuid();
			ImGui::SameLine();
			ImGui::LabelText("##UUID", "%s", uuids::to_string(uuid).data());
			ImGui::Separator();

			ObjectTransform();
			ObjectMesh();
			ObjectLight();
			ObjectLuaScript();

			ImGui::Separator();
			if (ImGui::Button("Add Component")) {
				ImGui::OpenPopup("add_component_popup");
			}

			if (ImGui::BeginPopupContextItem("add_component_popup")) {
				if (ImGui::MenuItem("Mesh")) {
					auto [ shaderId, shader ] = ShaderManager::instance().Create();
						shader.addVertexShader("shaders/basic.vs.glsl")
							.addFragmentShader("shaders/basic.fs.glsl")
							.link();

					// TODO: Recycle existing shaders
					shader.bind();
					shader.setUniform1i("material.albedo", 0);
					shader.setUniform1i("material.metallicRoughness", 1);
					shader.setUniform1i("material.normal", 2);
					shader.unbind();

					_currentEntity->AddComponents<ModelComponent>();

					auto &model = _currentEntity->Get<ModelComponent>();
					model.Shader = shaderId;
				}
				if (ImGui::MenuItem("Transform")) {
					_currentEntity->AddComponents<TransformComponent>();
				}
				if (ImGui::MenuItem("Lua Script")) {
					_currentEntity->AddComponents<LuaScriptComponent>();
				}
				if (ImGui::MenuItem("Child Entity")) {
				}
				ImGui::EndPopup();
			}

		ImGui::End();
	}

	void PlayMenu()
	{
		if (ImGui::Begin("Editor Actions")) {
			ImGui::PushItemWidth(-1);
			if (opfor::Engine::Instance().IsPlaying()) {
				if (ImGui::Button("Pause")) {
				}
				ImGui::SameLine();
				if (ImGui::Button("Stop")) {
					opfor::Engine::Instance().StopPlaying();
				}
			}
			else {
				if (ImGui::Button("Play")) {
					opfor::Engine::Instance().StartPlaying();
				}
			}
			ImGui::PopItemWidth();
		}
		ImGui::End();
	}

public:
	ImguiSystem() : _display(nullptr), _logAutoScroll(true), _currentEntity(nullptr)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		// Enable Docking
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		auto &io = ImGui::GetIO();

		ImFont *roboto = io.Fonts->AddFontFromFileTTF("thirdparty/imgui/misc/fonts/Roboto-Medium.ttf", 16);
		io.FontDefault = roboto;
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

		bool b = true;

		BeginFrame();
		BeginDockspace();
			ImGui::ShowDemoWindow(&b);
			MenuBar();
			PlayMenu();
			DrawGuizmoSelectedEnt();
			Materials();
			Log();
			SceneHierarchy();
			Properties();
		EndDockspace();
		EndFrame();

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
