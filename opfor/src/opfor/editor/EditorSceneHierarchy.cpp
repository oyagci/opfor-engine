#include "EditorSceneHierarchy.hpp"
#include <fmt/format.h>
#include <imgui.h>
#include <opfor/core/Application.hpp>
#include <opfor/editor/Editor.hpp>
#include <components/TransformComponent.hpp>

namespace opfor
{

void EditorSceneHierarchy::DrawHierarchy(EntityHierarchy const &hierarchy, size_t idx = 0) const
{
    size_t itemIndex = idx;
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

    bool clicked = false;

    if (hierarchy.children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (hierarchy.selected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (ImGui::TreeNodeEx(reinterpret_cast<void *>(itemIndex++), flags, "%s",
                          fmt::format("{}", hierarchy.name).c_str()))
    {
        clicked = ImGui::IsItemClicked();

        for (auto const &child : hierarchy.children)
        {
            DrawHierarchy(*child, itemIndex);
            itemIndex++;
        }
        ImGui::TreePop();
    }
    else
    {
        clicked = ImGui::IsItemClicked();
    }

    if (clicked)
    {
        Editor::SelectEntity(hierarchy.uuid);
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
                // auto ent = Application::Get().GetCurrentLevel()->CreateEntity();
                //(void)ent;
            }
            if (ImGui::MenuItem("Point Light"))
            {
                // auto ent = Application::Get().GetCurrentLevel()->CreateEntity();
                // ent->AddComponents<PointLightComponent>();
                //(void)ent;
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }

    auto allEnts = Application::Get().GetAllEntities();

    UnorderedMap<uuids::uuid, EntityHierarchy> entitiesByUuid;
    Vector<EntityHierarchy *> roots;

    for (auto const *ent : allEnts)
    {
        if (ent->HasComponents<TransformComponent>())
        {
            entitiesByUuid[ent->GetUuid()] = EntityHierarchy{ent->GetName(), ent->GetUuid(), ent, false, {}};
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

    if (!Editor::Selection().empty())
    {
        entitiesByUuid[Editor::Selection()[0]->GetUuid()].selected = true;
    }

    for (auto const &ent : roots)
    {
        DrawHierarchy(*ent);
    }

    ImGui::End();
}

} // namespace opfor
