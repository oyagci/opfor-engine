#include "EditorInspector.hpp"

#include "Editor.hpp"
#include "EditorMinProperty.hpp"
#include "EditorMaxProperty.hpp"
#include "EditorVisibleProperty.hpp"
#include "EditorUtils.hpp"
#include "CustomEditorProperty.hpp"

#include <components/ModelComponent.hpp>
#include <components/PointLightComponent.hpp>
#include <components/TransformComponent.hpp>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <opfor/core/Application.hpp>
#include <opfor/ecs/Entity.hpp>
#include <opfor/renderer/ShaderManager.hpp>
#include <uuid.h>
#include <regex>

opfor::UniquePtr<char[]> GetCwd();

namespace opfor
{

String EditorInspector::FormatName(String original)
{
    const String::size_type componentIdx = original.rfind("Component");

    // Remove last "Component" part of the string
    if (componentIdx != String::npos && componentIdx != 0 && (original.length() - (componentIdx + 9)) == 0)
    {
        original = original.substr(0, componentIdx);
    }

    std::string splitCamelCase;
    const std::regex rCamelCase("([a-z0-9])([A-Z])");
    std::regex_replace(std::back_inserter(splitCamelCase), original.begin(), original.end(), rCamelCase, "$1 $2");

    std::string splitSnakeCase;
    const std::regex rSnakeCase("([a-z0-9])_([A-Za-z])");
    std::regex_replace(std::back_inserter(splitSnakeCase), splitCamelCase.begin(), splitCamelCase.end(), rSnakeCase,
                       "$1 $2");

    // Capitalize
    bool prevWasSpace = true;
    for (auto &c : splitSnakeCase)
    {
        if (prevWasSpace && !std::isspace(c))
        {
            c = static_cast<char>(std::toupper(c));
            prevWasSpace = false;
        }
        else if (std::isspace(c))
        {
            prevWasSpace = true;
        }
    }

    return splitSnakeCase;
}

void EditorInspector::DrawComponentControls(ecs::IComponentBase &component)
{
    auto const &archetype = component.getArchetype();

    if (!ImGui::CollapsingHeader(fmt::format("{}##{}", FormatName(archetype.name), "").c_str()))
    {
        return;
    }

    auto const *customEditor = archetype.getProperty<CustomEditor>();

    if (customEditor)
    {
        customEditor->GUI()->target = &component;
        (*customEditor)();
        customEditor->GUI()->target = nullptr;
    }
    else
    {
        for (auto const &field : archetype.fields)
        {
            auto const *editorVisible = field.getProperty<EditorVisible>();

            if (editorVisible)
            {
                String name = editorVisible->editorName.value_or(FormatName(field.name));

                DrawDefaultControl(name, component, field);
            }
        }
    }
}

void EditorInspector::OnDrawGUI()
{
    if (ImGui::Begin("Inspector"))
    {
        if (Editor::Selection().empty())
        {
            ImGui::End();
            return;
        }

        ecs::IEntityBase *currentEntity = Editor::Selection()[0];

        std::string entityName = currentEntity->GetName();
        if (ImGui::InputText("##EntityName", &entityName))
        {
            currentEntity->SetName(entityName);
        }

        uuids::uuid const &uuid = currentEntity->GetUuid();
        ImGui::SameLine();
        ImGui::LabelText("##UUID", "%s", uuids::to_string(uuid).data());
        ImGui::Separator();

        for (auto const &[typeIdx, component] : *currentEntity)
        {
            DrawComponentControls(*component);
        }

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

void EditorInspector::DrawDefaultControl(String const &name, ecs::IComponentBase &component,
                                         rfk::Field const &field)
{
    auto const *min = field.getProperty<Min>();
    auto const *max = field.getProperty<Max>();
    bool const readonly = field.getProperty<ReadOnly>() == nullptr;

    if (*field.type.archetype == Vec3::staticGetArchetype())
    {
        auto &&vec = field.getData<Vec3>(&component);
        auto newValue = vec;

        EditorUtils::DrawVec3Control(name, newValue);

        if (newValue != vec)
        {
            field.setData<Vec3>(&component, std::move(newValue));
        }
    }
    else if (*field.type.archetype == Quat::staticGetArchetype())
    {
        // TODO: Fix float stability

        const auto rotator = field.getData<Quat>(&component).AsRotator();
        Vec3 euler{math::ClampAxis(rotator.Roll), math::ClampAxis(rotator.Yaw),
                          math::ClampAxis(rotator.Pitch)};

        if (EditorUtils::DrawVec3Control(name, euler))
        {
            const Vec3 normalized(math::ClampAxis(euler.x), math::ClampAxis(euler.y),
                                         math::ClampAxis(euler.z));

            field.setData<Quat>(&component, Rotator(normalized).AsQuaternion());
        }
    }
    else if (*field.type.archetype == *rfk::getArchetype<int>())
    {
        int &&value = field.getData<int>(&component);

        int newValue = value;
        const int original = value;
        EditorUtils::DrawIntControl(name, newValue, readonly, 1, min ? min->Get() : 0, max ? max->Get() : 0);

        if (min && value < min->Get())
        {
            value = min->Get();
        }

        if (max && value > max->Get())
        {
            value = max->Get();
        }

        if (value != original)
        {
            field.setData<int>(&component, std::move(value));
        }
    }
    else if (*field.type.archetype == *rfk::getArchetype<unsigned int>())
    {
        unsigned int &&value = field.getData<unsigned int>(&component);

        int newValue = value;
        const int original = value;
        EditorUtils::DrawIntControl(name, newValue, readonly, 1, min ? (std::max)(min->Get(), 0.0f) : 0.0f,
                                           max ? max->Get() : 0.0f);

        if (min && value < min->Get())
        {
            value = min->Get();
        }

        if (max && value > max->Get())
        {
            value = max->Get();
        }

        if (value != original)
        {
            field.setData<int>(&component, std::move(value));
        }
    }
    else if (*field.type.archetype == *rfk::getArchetype<float>())
    {
        float &&value = field.getData<float>(&component);

        float newValue = value;
        const float original = value;
        EditorUtils::DrawFloatControl(name, newValue, 0.1f, min ? min->Get() : 0.0f, max ? max->Get() : 0.0f);
        constexpr float epsilon = std::numeric_limits<float>::epsilon();
        if (abs(value - original) > epsilon)
        {
            field.setData<float>(&component, std::move(value));
        }
    }
    else if (*field.type.archetype == String::staticGetArchetype())
    {
        auto &&value = field.getData<String>(&component);

        auto newValue = value;
        EditorUtils::DrawStringControl(name, newValue, readonly);
        if (newValue != value)
        {
            field.setData<String>(&component, std::move(newValue));
        }
    }
}

} // namespace opfor
