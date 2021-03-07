#include "EditorSceneHierarchy.hpp"
#include "core/Application.hpp"
#include "fmt/format.h"
#include "imgui.h"
#include "layers/ImGuiLayer.hpp"

void EditorSceneHierarchy::DrawHierarchy(opfor::UnorderedMap<uuids::uuid, EntityHierarchy> const &lookup,
                                         EntityHierarchy const &hierarchy, int idx = 0) const
{
    size_t itemIndex = idx;
    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (ImGui::TreeNodeEx(reinterpret_cast<void *>(itemIndex++), flags, "%s",
                          fmt::format("{}", hierarchy.name).c_str()))
    {
        for (auto const &child : hierarchy.children)
        {
            DrawHierarchy(lookup, child, itemIndex);
            itemIndex++;
        }
        ImGui::TreePop();
    }
    if (ImGui::IsItemClicked())
    {
        //_selectedItem = static_cast<int>(itemIndex);
        // ImGuiLayer::Get().SetSelectedEntity(allEnts[itemIndex]);
        // opfor::Application::Get().OnSelectItem(_selectedItem);
    }
}

void EditorSceneHierarchy::OnDrawGUI()
{
    ImGui::Begin("Scene Hierarchy");

    if (ImGui::BeginPopupContextWindow("hierarchy_popup_menu"))
    {
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("hierarchy_popup_menu"))
    {
        if (ImGui::BeginMenu("Add..."))
        {
            if (ImGui::MenuItem("Entity"))
            {
                // auto ent = opfor::Application::Get().GetCurrentLevel()->CreateEntity();
                //(void)ent;
            }
            if (ImGui::MenuItem("Point Light"))
            {
                // auto ent = opfor::Application::Get().GetCurrentLevel()->CreateEntity();
                // ent->AddComponents<PointLightComponent>();
                //(void)ent;
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }

    auto allEnts = opfor::Application::Get().GetAllEntities();

    opfor::UnorderedMap<uuids::uuid, ecs::IEntityBase *> entitiesByUuid;
    opfor::UnorderedMap<uuids::uuid, EntityHierarchy> lookup;

    for (auto const *ent : allEnts)
    {
        if (ent->HasComponents<TransformComponent>())
        {
            auto &t = ent->Get<TransformComponent>();

            lookup[ent->GetUuid()].name = ent->GetName();
            lookup[ent->GetUuid()].uuid = ent->GetUuid();

            if (t.parent)
            {
                lookup[ent->GetUuid()].parent = t.parent->get().entity.GetUuid();
            }
        }
    }

    for (auto const &[uuid, entityHierarchy] : lookup)
    {
        if (entityHierarchy.parent)
        {
            lookup[entityHierarchy.parent.value()].children.push_back(entityHierarchy);
        }
    }

    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    size_t itemIndex = 0;

    for (auto const &[uuid, ent] : lookup)
    {
        if (!ent.parent)
        {
            DrawHierarchy(lookup, ent);
        }

        itemIndex++;
    }

    // size_t itemIndex = 0;
    // for (auto const &ent : allEnts) {
    //	auto name = ent->GetName();
    //	if (ImGui::TreeNodeEx(reinterpret_cast<void*>(itemIndex), flags,fmt::format("{}", name).c_str())) {
    //		ImGui::TreePop();
    //	}
    //	if (ImGui::IsItemClicked()) {
    //		_selectedItem = itemIndex;
    //		ImGuiLayer::Get().SetSelectedEntity(allEnts[itemIndex]);
    //		opfor::Application::Get().OnSelectItem(_selectedItem);
    //	}
    //	itemIndex++;
    //}

    ImGui::End();
}
