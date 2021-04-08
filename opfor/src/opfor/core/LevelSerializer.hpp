#pragma once

#include <opfor/ILevel.hpp>
#include <json.hpp>
#include <opfor/core/Application.hpp>
#include <opfor/renderer/Model.hpp>
#include <opfor/renderer/ShaderManager.hpp>
#include <fstream>
#include <glm/vec3.hpp>

#include <components/ModelComponent.hpp>
#include <components/PointLightComponent.hpp>
#include <components/SkyboxComponent.hpp>
#include <components/TransformComponent.hpp>

#include "uuid.h"
using json = nlohmann::json;

namespace opfor
{

class LevelSerializer
{
    std::unordered_map<uuids::uuid, ecs::IEntityBase *> _entities;
    std::unordered_map<uuids::uuid, ModelComponent> _models;

  public:
    static std::string ReadFile(const std::string &path)
    {
        std::stringstream result;
        std::ifstream in(path);

        if (in.is_open() && !in.bad())
        {
            result << in.rdbuf();
            in.close();
            return result.str();
        }

        OP4_CORE_ERROR("Failed to read file: {}", path);
        return "";
    }

    static std::string Serialize()
    {
        json serialized = json::array();

        // auto entities = level.GetEntities();

        auto entities = Application::Get().GetAllEntities();

        for (size_t n = 0; n < entities.size(); n++)
        {
            auto &ent = entities[n];

            serialized[n]["guid"] = to_string(ent->GetUuid());
            serialized[n]["name"] = ent->GetName();

            if (ent->HasComponents<TransformComponent>())
            {
                auto &transform = ent->Get<TransformComponent>();

                serialized[n]["transform"]["position"] = {transform.position.x, transform.position.y,
                                                          transform.position.z};
                serialized[n]["transform"]["rotation"] = {transform.rotation.w, transform.rotation.x,
                                                          transform.rotation.y, transform.rotation.z};
                serialized[n]["transform"]["scale"] = {transform.scale.x, transform.scale.y, transform.scale.z};
            }

            if (ent->HasComponents<SkyboxComponent>())
            {
                serialized[n]["skybox"] = true;
            }
        }

        return serialized.dump(2);
    }

    ///
    /// Deserialize a level from a level JSON
    ///
    // void Deserialize(std::string const &path)
    //{
    //	auto [ shaderId, meshShader ] = ShaderManager::Get().Create("shaders/basic.glsl");

    //	std::string const levelJsonFile = ReadFile(path);
    //	auto levelJson = json::parse(levelJsonFile);

    //	for (auto const &model : levelJson["models"]) {
    //		ModelComponent component{};
    //		opfor::Model engineModel;

    //		auto uuidstr = model["uuid"].get<std::string>();
    //		auto uuid = uuids::uuid::from_string(uuidstr).value();

    //		engineModel.LoadFromGLTF(model["uri"].get<std::string>());

    //		component.Meshes.insert(component.Meshes.begin(),
    //			engineModel.GetMeshes().begin(),
    //			engineModel.GetMeshes().end());

    //		_models[uuid] = component;
    //	}

    //	for (auto const &ent : levelJson["entities"]) {

    //		auto entity = CreateEntity();

    //		entity->SetName(ent["name"]);

    //		for (auto const &component : ent["components"]) {
    //			std::string type = component["type"];

    //			if (type == "MODEL") {
    //				auto uuid = uuids::uuid::from_string(component["uuid"].get<std::string>());

    //				if (!uuid.has_value()) {
    //					Logger::Error("Entity does not have a UUID\n");
    //					continue ;
    //				}

    //				entity->AddComponents<ModelComponent>();
    //				entity->Get<ModelComponent>() = _models[uuid.value()];
    //			}
    //			else if (type == "POINTLIGHT") {
    //				entity->AddComponents<PointLightComponent>();

    //				auto &pl = entity->Get<PointLightComponent>();
    //				std::array<float, 3> color = component["color"];
    //				pl.Color = { color[0], color[1], color[2] };
    //				pl.Intensity = component["intensity"];
    //			}
    //			else if (type == "TRANSFORM") {
    //				auto transform(component["transform"]);
    //				std::array<float, 3> position = transform["position"];
    //				//std::array<float, 3> rotation = transform["rotation"];
    //				std::array<float, 3> scale = transform["scale"];

    //				entity->AddComponents<TransformComponent>();
    //				auto &tr = entity->Get<TransformComponent>();
    //				tr.position = { position[0], position[1], position[2] };
    //				//tr.rotation = { rotation[0], rotation[1], rotation[2] };
    //				tr.scale = { scale[0], scale[1], scale[2] };
    //			}
    //		}
    //	}
    //}

    static void Serialize(std::string const &filename)
    {
    }
};

} // namespace opfor
