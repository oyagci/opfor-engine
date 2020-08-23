#include "ImGuiLayer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <string>
#ifdef OP4_PLATFORM_LINUX
# include <unistd.h>
#elif defined(OP4_PLATFORM_WINDOWS)
# include <direct.h>
#endif

#include "opfor/core/Application.hpp"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_glfw.h"
#include "misc/cpp/imgui_stdlib.h"
#include "ImGuizmo.h"
#include "nfd.hpp"
#include "opfor/renderer/ShaderManager.hpp"

#include "components/PlayerCameraComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/ModelComponent.hpp"

ImGuiLayer::ImGuiLayer() = default;

std::unique_ptr<char[]> GetCwd()
{
#ifdef OP4_PLATFORM_LINUX
	auto buf = getcwd(nullptr, 0);

	auto ret = std::make_unique<char[]>(strlen(buf));
	strcpy(ret.get(), buf);

	return ret;
#elif defined(OP4_PLATFORM_WINDOWS)
	DWORD bufLen = GetCurrentDirectory(0, nullptr);
	auto buffer = std::make_unique<WCHAR[]>(bufLen);

	//std::wcstombs
	GetCurrentDirectory(bufLen, buffer.get());

	auto ret = std::make_unique<char[]>(bufLen);
	
	size_t pRetVal = 0;
	

	wcstombs_s(&pRetVal, ret.get(), bufLen, buffer.get(), bufLen);

	return ret;
#else
# error "Unsupported Platform!"
#endif
}

void ImGuiLayer::BeginFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::EndFrame()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::DrawGuizmoSelectedEnt()
{
	auto cameraEnt = opfor::Application::Get().GetEntities<PlayerCameraComponent>()[0];
	auto camera = cameraEnt->Get<PlayerCameraComponent>();
	auto selectedEnt = opfor::Application::Get().GetEntities<TransformComponent, SelectedComponent>();
	if (selectedEnt.size() > 0) {

		auto selected = selectedEnt[0]->Get<TransformComponent>();

		glm::mat4 model(1.0f);
		model = glm::translate(model, selected.position);
		model = glm::scale(model, selected.scale);

		bool changed = false;

		ImGuizmo::BeginFrame();
		ImGuizmo::SetRect(_ViewportPosition.x, _ViewportPosition.y, _ViewportSize.x, _ViewportSize.y);
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

void ImGuiLayer::BeginDockspace()
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

void ImGuiLayer::EndDockspace()
{
	ImGui::End();
}

void ImGuiLayer::MenuBar()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Close")) {
				opfor::Application::Get().Close();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("New Level")) {
			}
			if (ImGui::MenuItem("Open Level")) {
				char *outPath = nullptr;
				NFD_OpenDialog(nullptr, GetCwd().get(), &outPath);
				if (outPath) {
					opfor::Application::Get().LoadLevel(outPath);
				}
			}
			if (ImGui::MenuItem("Save Level")) {
			}
			if (ImGui::MenuItem("Save Level As...")) {
				char *outPath = nullptr;
				NFD_OpenDialog(nullptr, GetCwd().get(), &outPath);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Lighting")) {
			if (ImGui::MenuItem("Build")) {
				/* Build Lighting for the Level */
				opfor::Application::Get().OnBuildLighting();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void ImGuiLayer::Materials()
{
	auto materialList = opfor::Application::Get().GetMaterialList();
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

void ImGuiLayer::Log()
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
	clipper.Begin(static_cast<int>(lineOffsets.size()));
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

void ImGuiLayer::SceneHierarchy()
{
	ImGui::Begin("Scene Hierarchy");

	if (ImGui::BeginPopupContextWindow("hierarchy_popup_menu")) {
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("hierarchy_popup_menu")) {
		if (ImGui::BeginMenu("Add...")) {
			if (ImGui::MenuItem("Entity")) {
				auto ent = opfor::Application::Get().GetCurrentLevel()->CreateEntity();
				(void)ent;
			}
			if (ImGui::MenuItem("Point Light")) {
				auto ent = opfor::Application::Get().GetCurrentLevel()->CreateEntity();
				ent->AddComponents<PointLightComponent>();
				(void)ent;
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	auto allEnts = opfor::Application::Get().GetAllEntities();

	size_t itemIndex = 0;
	for (auto const &ent : allEnts) {
		auto name = ent->GetName();
		name += "##" + std::to_string(itemIndex);
		if (ImGui::Selectable(name.c_str(), _SelectedItem == itemIndex)) {
			Logger::Verbose("Clicked on item {} (prev. {}) (ID. {})\n",
				itemIndex, _SelectedItem, allEnts[itemIndex]->GetId());
			_SelectedItem = itemIndex;
			_currentEntity = allEnts[itemIndex];
			opfor::Application::Get().OnSelectItem(_SelectedItem);
		}
		itemIndex++;
	}

	ImGui::End();
}

void ImGuiLayer::ObjectTransform()
{
	if (!_currentEntity) { return ; }
	if (!_currentEntity->HasComponents<TransformComponent>()) { return; }

	if (!ImGui::CollapsingHeader("Transform")) { return ; }

	bool changed = false;

	auto &transform = _currentEntity->Get<TransformComponent>();

	std::array<float, 3> rotation{};
	std::array<float, 3> translation{ transform.position.x, transform.position.y, transform.position.z };
	std::array<float, 3> scale{ transform.scale.x, transform.scale.y, transform.scale.z };

	if (ImGui::InputFloat3("Position", translation.data(), 3)) {
		changed = true;
	}
	if (ImGui::InputFloat3("Rotation", rotation.data(), 3)) {
		changed = true;
	}
	if (ImGui::InputFloat3("Scale", scale.data(), 3)) {
		changed = true;
	}

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

void ImGuiLayer::ObjectMesh()
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
		if (NFD_OpenDialog(nullptr, GetCwd().get(), &newPath) == NFD_OKAY
			&& newPath) {
			model.Path = std::string(newPath);
			opfor::Application::Get().OnRebuildModel(model);
		}
	}

	ImGui::Columns(1);
}

void ImGuiLayer::ObjectLight()
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

void ImGuiLayer::Properties()
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

		ImGui::Separator();
		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("add_component_popup");
		}

		if (ImGui::BeginPopupContextItem("add_component_popup")) {
			if (ImGui::MenuItem("Mesh")) {
				auto [ shaderId, shader ] = ShaderManager::Get().Create("shaders/basic.glsl");

				// TODO: Recycle existing shaders
				shader.Bind();
				shader.SetUniform("material.albedo", 0);
				shader.SetUniform("material.metallicRoughness", 1);
				shader.SetUniform("material.normal", 2);
				shader.Unbind();

				_currentEntity->AddComponents<ModelComponent>();

				auto &model = _currentEntity->Get<ModelComponent>();
				model.Shader = shaderId;
			}
			if (ImGui::MenuItem("Transform")) {
				_currentEntity->AddComponents<TransformComponent>();
			}
			if (ImGui::MenuItem("Child Entity")) {
			}
			ImGui::EndPopup();
		}

	ImGui::End();
}

void ImGuiLayer::PlayMenu()
{
	if (ImGui::Begin("Editor Actions")) {
		ImGui::PushItemWidth(-1);
		if (opfor::Application::Get().IsPlaying()) {
			if (ImGui::Button("Pause")) {
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop")) {
				opfor::Application::Get().StopPlaying();
			}
		}
		else {
			if (ImGui::Button("Play")) {
				opfor::Application::Get().StartPlaying();
			}
		}
		ImGui::PopItemWidth();
	}
	ImGui::End();
}

void ImGuiLayer::Viewport()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar;

	if (ImGui::Begin("Viewport", nullptr, windowFlags)) {

		_ViewportSize = ImGui::GetWindowSize();
		_ViewportPosition = ImGui::GetWindowPos();

		ImVec2 winSize = ImGui::GetWindowSize();

		// TODO: Automatically adjust to viewport's aspect ratio.

		float targetAspectRatio = 16.0f / 9.0f;

		// I would have loved to just reinterpret_cast to ImTextureID but the compiler won't let me :(
		uint32_t rawHandle = opfor::Application::Get().GetViewportTexture()->GetRawHandle();
		ImTextureID *rawHandleP = (ImTextureID*)&rawHandle;

		if ((winSize.x / winSize.y) < targetAspectRatio) {
			ImGui::Image(*rawHandleP,
				{ winSize.x, winSize.x / targetAspectRatio },
				ImVec2(0.0f, 1.0f),
				ImVec2(1.0f, 0.0f),
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
				ImVec4(1.0f, 1.0f, 1.0f, 0.0f));

			_ViewportSize.y = winSize.x / targetAspectRatio;
		}
		else {
			ImGui::Image(*rawHandleP,
				{ winSize.y * targetAspectRatio, winSize.y },
				ImVec2(0.0f, 1.0f),
				ImVec2(1.0f, 0.0f),
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
				ImVec4(1.0f, 1.0f, 1.0f, 0.0f));

			_ViewportSize.x = winSize.y * targetAspectRatio;
		}

		DrawGuizmoSelectedEnt();
	}
	ImGui::End();

	ImGui::PopStyleVar();
}

void ImGuiLayer::SetupImGuiStyle()
{
	ImGuiStyle & style = ImGui::GetStyle();
	ImVec4 * colors = style.Colors;

	/// 0 = FLAT APPEARENCE
	/// 1 = MORE "3D" LOOK
	int is3D = 1;

	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_Border]                 = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
	colors[ImGuiCol_Separator]              = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

	style.PopupRounding = 3;

	style.WindowPadding = ImVec2(4, 4);
	style.FramePadding  = ImVec2(6, 4);
	style.ItemSpacing   = ImVec2(6, 2);

	style.ScrollbarSize = 18;

	style.WindowBorderSize = 1;
	style.ChildBorderSize  = 1;
	style.PopupBorderSize  = 1;
	style.FrameBorderSize  = static_cast<float>(is3D);

	style.WindowRounding    = 0;
	style.ChildRounding     = 0;
	style.FrameRounding     = 0;
	style.ScrollbarRounding = 2;
	style.GrabRounding      = 0;

	#ifdef IMGUI_HAS_DOCK
		style.TabBorderSize = static_cast<float>(is3D);
		style.TabRounding   = 0;

		colors[ImGuiCol_DockingEmptyBg]     = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		colors[ImGuiCol_Tab]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_TabHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_TabActive]          = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		colors[ImGuiCol_TabUnfocused]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		colors[ImGuiCol_DockingPreview]     = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	#endif
}

void ImGuiLayer::OnAttach()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	SetupImGuiStyle();

	// Enable Docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	auto &io = ImGui::GetIO();

	//ImFont *roboto = io.Fonts->AddFontFromFileTTF("vendor/imgui/misc/fonts/Roboto-Medium.ttf", 16);
	//io.FontDefault = roboto;

	ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(opfor::Application::Get().GetWindow()->GetRawHandle()), true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

void ImGuiLayer::OnDetach()
{
}

void ImGuiLayer::OnImGuiRender()
{
	static bool show = true;

	BeginFrame();
	BeginDockspace();
		ImGui::ShowDemoWindow(&show);
		Viewport();
		MenuBar();
		PlayMenu();
		Materials();
		Log();
		SceneHierarchy();
		Properties();
	EndDockspace();
	EndFrame();

	if (opfor::Input::GetKeyDown(opfor::KeyCode::Escape)) {
		auto playerEnts = opfor::Application::Get().GetEntities<PlayerCameraComponent>();
		if (playerEnts.size() > 0) {
			auto camera = playerEnts[0]->Get<PlayerCameraComponent>();

			// Enable/Disable input for camera
			camera.useInput = !camera.useInput;
			playerEnts[0]->Set(camera);

			// Enable/Disable cursor
			opfor::Application::Get().GetWindow()->HideCursor(camera.useInput);
		}
	}

	auto mouse = opfor::Input::GetMouseButton(opfor::MouseButton::ButtonRight) == opfor::KeyStatus::Pressed;
	if (mouse) {
		opfor::Application::Get().GetWindow()->HideCursor(true);
	}
	else {
		opfor::Application::Get().GetWindow()->HideCursor(false);
	}
}
