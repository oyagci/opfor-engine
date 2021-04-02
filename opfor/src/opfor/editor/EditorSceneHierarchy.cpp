#include "EditorSceneHierarchy.hpp"
#include <fmt/format.h>
#include <imgui.h>
#include <opfor/core/Application.hpp>
#include <opfor/layers/ImGuiLayer.hpp>

void EditorSceneHierarchy::DrawHierarchy(EntityHierarchy const &hierarchy, size_t idx = 0) const
{
    size_t itemIndex = idx;
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (hierarchy.children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (ImGui::TreeNodeEx(reinterpret_cast<void *>(itemIndex++), flags, "%s",
                          fmt::format("{}", hierarchy.name).c_str()))
    {
        for (auto const &child : hierarchy.children)
        {
            DrawHierarchy(*child, itemIndex);
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

    opfor::UnorderedMap<uuids::uuid, EntityHierarchy> entitiesByUuid;
    opfor::Vector<EntityHierarchy*> roots;

    for (auto const *ent : allEnts)
    {
        if (ent->HasComponents<TransformComponent>())
        {
            entitiesByUuid[ent->GetUuid()] = EntityHierarchy{ent->GetName(), ent->GetUuid(), ent, {}};
        }
    }

    for (auto &[uuid, ent] : entitiesByUuid)
    {
        if (!ent.entity->HasComponents<TransformComponent>())
        {
            continue;
        }

        auto const &t = ent.entity->Get<TransformComponent>();

        if (!t.parent)
        {
            roots.emplace_back(&ent);
        }
        else if (entitiesByUuid.find(t.parent->get().entity.GetUuid()) != entitiesByUuid.end())
        {
            entitiesByUuid[t.parent->get().entity.GetUuid()].children.push_back(&ent);
        }
    }

    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    size_t itemIndex = 0;

    for (auto const &ent : roots)
    {
        DrawHierarchy(*ent);
        itemIndex++;
    }

    ImGui::End();
}
