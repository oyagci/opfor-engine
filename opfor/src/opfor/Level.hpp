#pragma once

#include "ILevel.hpp"
#include "json.hpp"
#include "opfor/core/Application.hpp"
#include "opfor/renderer/Model.hpp"
#include "opfor/renderer/ShaderManager.hpp"
#include <fstream>
#include <glm/vec3.hpp>

#include "components/ModelComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/TransformComponent.hpp"

#include "uuid.h"

using json = nlohmann::json;

class Level : public opfor::ILevel
{
    unsigned int _meshShader;
    std::unordered_map<uuids::uuid, ecs::IEntityBase *> _entities;
    bool _isLoaded = false;

    std::unordered_map<uuids::uuid, ModelComponent> _models;

  public:
    std::string ReadFile(const std::string &path)
    {
        std::stringstream result;
        std::ifstream in(path);

        if (in.is_open() && !in.bad())
        {
            result << in.rdbuf();
            in.close();
            return result.str();
        }

        fmt::print(stderr, "Failed to read file: {}\n", path);
        return "";
    }

    ///
    /// Load a level from a level JSON
    ///
    void Load(std::string const &path)
    {
        if (_isLoaded)
        {
            Unload();
        }

        auto [shaderId, meshShader] = ShaderManager::Get().Create("shaders/basic.glsl");

        meshShader.Bind();
        meshShader.SetUniform("material.albedo", 0);
        meshShader.SetUniform("material.metallicRoughness", 1);
        meshShader.SetUniform("material.normal", 2);
        meshShader.SetUniform("material.metallicFactor", 1.0f);
        meshShader.SetUniform("material.roughnessFactor", 1.0f);
        meshShader.Unbind();
        _meshShader = shaderId;

        std::string const levelJsonFile = ReadFile(path);
        auto levelJson = json::parse(levelJsonFile);

        for (auto const &model : levelJson["models"])
        {
            ModelComponent component{};
            opfor::Model engineModel;

            auto uuidstr = model["uuid"].get<std::string>();
            auto uuid = uuids::uuid::from_string(uuidstr).value();

            engineModel.LoadFromGLTF(model["uri"].get<std::string>());

            component.meshes.insert(component.meshes.begin(), engineModel.GetMeshes().begin(),
                                    engineModel.GetMeshes().end());
            component.shader = _meshShader;

            _models[uuid] = component;
        }

        for (auto const &ent : levelJson["entities"])
        {

            auto entity = CreateEntity();

            entity->SetName(ent["name"]);

            for (auto const &component : ent["components"])
            {
                std::string type = component["type"];

                if (type == "MODEL")
                {
                    auto uuid = uuids::uuid::from_string(component["uuid"].get<std::string>());

                    if (!uuid.has_value())
                    {
                        Logger::Error("Entity does not have a UUID\n");
                        continue;
                    }

                    entity->AddComponents<ModelComponent>();
                    entity->Get<ModelComponent>() = _models[uuid.value()];
                }
                else if (type == "POINTLIGHT")
                {
                    entity->AddComponents<PointLightComponent>();

                    auto &pl = entity->Get<PointLightComponent>();
                    std::array<float, 3> color = component["color"];
                    pl.Color = {color[0], color[1], color[2]};
                    pl.Intensity = component["intensity"];
                }
                else if (type == "TRANSFORM")
                {
                    auto transform(component["transform"]);
                    std::array<float, 3> position = transform["position"];
                    // std::array<float, 3> rotation = transform["rotation"];
                    std::array<float, 3> scale = transform["scale"];

                    entity->AddComponents<TransformComponent>();
                    auto &tr = entity->Get<TransformComponent>();
                    tr.position = {position[0], position[1], position[2]};
                    // tr.rotation = { rotation[0], rotation[1], rotation[2] };
                    tr.scale = {scale[0], scale[1], scale[2]};
                }
            }
        }

        _isLoaded = true;
    }

    void Unload()
    {
        _isLoaded = false;

        for (auto &entity : _entities)
        {
            opfor::Application::Get().DeleteEntity(entity.second->GetId());
        }
        _entities.clear();
        _models.clear();

        ShaderManager::Get().Delete(_meshShader);

        _meshShader = 0;
    }

    ///
    /// Create an Entity.
    /// The lifetime of the created entity is managed by the Level (Load/Unload)
    ///
    ecs::IEntityBase *CreateEntity()
    {
        auto entity = opfor::Application::Get().CreateEntity<TransformComponent>();
        _entities[entity->GetUuid()] = entity;
        return entity;
    }
};
