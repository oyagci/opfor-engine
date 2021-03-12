#include "EditorInspector.hpp"
#include "components/ModelComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/Application.hpp"
#include "ecs/Entity.hpp"
#include "imgui.h"
#include "layers/ImGuiLayer.hpp"
#include "misc/cpp/imgui_stdlib.h"
#include "renderer/ShaderManager.hpp"
#include "uuid.h"
#include <nfd.h>

opfor::UniquePtr<char[]> GetCwd();

void EditorInspector::ObjectTransform(ecs::IEntityBase *entity)
{
    if (!entity->HasComponents<TransformComponent>())
    {
        return;
    }

    if (!ImGui::CollapsingHeader("Transform"))
    {
        return;
    }

    bool changed = false;

    auto &transform = entity->Get<TransformComponent>();

    std::array<float, 3> rotation{transform.rotation.x, transform.rotation.y, transform.rotation.z};
    std::array<float, 3> translation{transform.position.x, transform.position.y, transform.position.z};
    std::array<float, 3> scale{transform.scale.x, transform.scale.y, transform.scale.z};

    if (ImGui::InputFloat3("Position", translation.data(), 3))
    {
        changed = true;
    }
    if (ImGui::InputFloat3("Rotation", rotation.data(), 3))
    {
        changed = true;
    }
    if (ImGui::InputFloat3("Scale", scale.data(), 3))
    {
        changed = true;
    }

    if (ImGui::Button("Reset"))
    {
        rotation.fill(0.0f);
        translation.fill(0.0f);
        scale.fill(1.0f);
        changed = true;
    }

    if (changed)
    {
        transform.position = {translation[0], translation[1], translation[2]};
        transform.scale = {scale[0], scale[1], scale[2]};
    }
}

void EditorInspector::ObjectMesh(ecs::IEntityBase *entity)
{
    if (!entity->HasComponents<ModelComponent>())
    {
        return;
    }

    if (!ImGui::CollapsingHeader("Static Mesh"))
    {
        return;
    }

    auto &model = entity->Get<ModelComponent>();

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
    ImGui::Columns(2, NULL, true);

    ImGui::Text("File Path");
    ImGui::NextColumn();

    std::string inputFilePath = model.path;

    if (ImGui::InputText("##FilePath", &inputFilePath))
    {
    }
    ImGui::SameLine();
    if (ImGui::Button("..."))
    {
        char *newPath = nullptr;
        // Open File Dialog
        if (NFD_OpenDialog(nullptr, GetCwd().get(), &newPath) == NFD_OKAY && newPath)
        {
            model.path = std::string(newPath);
            opfor::Application::Get().OnRebuildModel(model);
        }
    }

    ImGui::Columns(1);
}

void EditorInspector::ObjectLight(ecs::IEntityBase *entity)
{
    if (!entity->HasComponents<PointLightComponent>())
    {
        return;
    }

    if (!ImGui::CollapsingHeader("Light"))
    {
        return;
    }

    auto &light = entity->Get<PointLightComponent>();

    std::array<const char *, 1> lightTypes = {"Point"};
    size_t currentTypeIdx = 0;
    const char *comboLabel = lightTypes[currentTypeIdx];

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
    ImGui::Columns(2, nullptr, true);

    ImGui::Text("Type");
    ImGui::NextColumn();
    if (ImGui::BeginCombo("##LightTypeCombo", comboLabel, 0))
    {
        for (size_t i = 0; i < lightTypes.size(); i++)
        {
            const bool isSelected = currentTypeIdx == i;
            if (ImGui::Selectable(lightTypes[i], isSelected))
            {
                currentTypeIdx = i;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::NextColumn();

    ImGui::Text("Color");
    ImGui::NextColumn();
    std::array<float, 3> color{light.Color[0], light.Color[1], light.Color[2]};
    if (ImGui::ColorEdit3("##LightColor", color.data()))
    {
        light.Color = {color[0], color[1], color[2]};
    }
    ImGui::NextColumn();

    ImGui::Text("Intensity");
    ImGui::NextColumn();
    float intensity = light.Intensity;
    if (ImGui::InputFloat("##LightIntensity", &intensity))
    {
        light.Intensity = intensity;
    }

    ImGui::Columns(1);
}

void EditorInspector::OnDrawGUI()
{
    ecs::IEntityBase *currentEntity = ImGuiLayer::Get().GetSelectedEntity();

    if (ImGui::Begin("Inspector"))
    {
        if (currentEntity == nullptr)
        {
            ImGui::End();
            return;
        }

        std::string entityName = currentEntity->GetName();
        if (ImGui::InputText("##EntityName", &entityName))
        {
            currentEntity->SetName(entityName);
        }

        uuids::uuid const &uuid = currentEntity->GetUuid();
        ImGui::SameLine();
        ImGui::LabelText("##UUID", "%s", uuids::to_string(uuid).data());
        ImGui::Separator();

        ObjectTransform(currentEntity);
        ObjectMesh(currentEntity);
        ObjectLight(currentEntity);

        ImGui::Separator();
        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("add_component_popup");
        }

        if (ImGui::BeginPopupContextItem("add_component_popup"))
        {
            if (ImGui::MenuItem("Mesh"))
            {
                auto [shaderId, shader] = ShaderManager::Get().Create("shaders/basic.glsl");

                // TODO: Recycle existing shaders
                shader.Bind();
                shader.SetUniform("material.albedo", 0);
                shader.SetUniform("material.metallicRoughness", 1);
                shader.SetUniform("material.normal", 2);
                shader.Unbind();

                currentEntity->AddComponents<ModelComponent>();

                auto &model = currentEntity->Get<ModelComponent>();
                model.shader = shaderId;
            }
            if (ImGui::MenuItem("Child Entity"))
            {
            }
            ImGui::EndPopup();
        }
        ImGui::End();
    }
}
