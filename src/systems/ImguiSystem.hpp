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
#include "Engine.hpp"
#include "Logger.hpp"

class ImguiSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Display *_display;

	bool _logAutoScroll;

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
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Close")) {
					engine::Engine::Instance().Close();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Lighting")) {
				if (ImGui::MenuItem("Build")) {
					/* Build Lighting for the Level */
					engine::Engine::Instance().OnBuildLighting();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
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

	void Materials()
	{
		auto materialList = engine::Engine::Instance().GetMaterialList();
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
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");

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

	void EntityList()
	{
		ImGui::Begin("Entities");

		auto allEnts = GetAllEntities();
		for (auto const &ent : allEnts) {
			auto name = ent->GetName();
			ImGui::Text("%s", name.c_str());
		}

		ImGui::End();
	}

public:
	ImguiSystem() : _display(nullptr), _logAutoScroll(true)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		// Enable Docking
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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

		BeginFrame();
		BeginDockspace();
			MenuBar();
			DrawGuizmoSelectedEnt();
			LightProperties();
			Materials();
			Log();
			EntityList();
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
