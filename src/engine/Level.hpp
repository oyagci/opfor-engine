#pragma once

#include "Engine.hpp"
#include "ILevel.hpp"
#include "Model.hpp"
#include "ShaderManager.hpp"
#include "json.hpp"
#include <glm/vec3.hpp>

#include "components/ModelComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/LuaScriptComponent.hpp"

#include "uuid.h"

using json = nlohmann::json;

class Level : public engine::ILevel
{
	unsigned int _meshShader;
	std::unordered_map<uuids::uuid, ecs::IEntityBase *> _entities;
	bool _isLoaded = false;

public:
	void Load(std::string const &path) override
	{
		if (_isLoaded) {
			Unload();
		}

		auto [ shaderId, meshShader ] = ShaderManager::instance().Create();
		meshShader.addVertexShader("./shaders/basic.vs.glsl")
			.addFragmentShader("./shaders/basic.fs.glsl")
			.link();

		meshShader.bind();
		meshShader.setUniform1i("material.albedo", 0);
		meshShader.setUniform1i("material.metallicRoughness", 1);
		meshShader.setUniform1i("material.normal", 2);
		meshShader.setUniform1f("material.metallicFactor", 1.0f);
		meshShader.setUniform1f("material.roughnessFactor", 1.0f);
		meshShader.unbind();
		_meshShader = shaderId;

		std::string const levelJsonFile = lazy::utils::LoadFile(path);
		auto levelJson = json::parse(levelJsonFile);

		for (auto const &model : levelJson["models"]) {
			auto entity = engine::Engine::Instance().CreateEntity<ModelComponent, TransformComponent>();
			auto engineModel = engine::Model();

			auto uuidstr = model["uuid"].get<std::string>();
			auto uuid = uuids::uuid::from_string(uuidstr).value();

			entity->SetName(model["name"]);

			engineModel.LoadFromGLTF(model["uri"].get<std::string>());

			auto &modelComponent = entity->Get<ModelComponent>();
				modelComponent.Meshes.insert(modelComponent.Meshes.begin(),
					engineModel.GetMeshes().begin(),
					engineModel.GetMeshes().end());
				modelComponent.Shader = _meshShader;

			auto const position = model["transform"]["position"].get<std::array<float, 3>>();
			//auto const rotation = model["transform"]["position"].get<std::array<float, 3>>();
			auto const scale = model["transform"]["scale"].get<std::array<float, 3>>();

			auto &modelTransform = entity->Get<TransformComponent>();
				modelTransform.position = { position[0], position[1], position[2] };
				modelTransform.scale = { scale[0], scale[1], scale[2] };

			_entities[uuid] = entity;
		}
//		for (auto const &light : levelJson["lights"]) {
//
//			if (light["type"].get<std::string>() == "POINT") {
//				auto entity = engine::Engine::Instance().CreateEntity<PointLightComponent, TransformComponent>();
//
//				auto color = light["color"].get<std::array<float, 3>>();
//
//				auto &lightProperties = entity->Get<PointLightComponent>();
//					lightProperties.Color = { color[0], color[1], color[2] };
//					lightProperties.Intensity = light["intensity"].get<float>();
//
//				auto position = light["transform"]["position"].get<std::array<float, 3>>();
//				auto &lightTransform = entity->Get<TransformComponent>();
//					lightTransform.position = { position[0], position[1], position[2] };
//
//				entity->SetName(light["name"]);
//
//				_entities.push_back(entity);
//			}
//		}
	}

	void Unload() override {
		_isLoaded = false;

		for (auto &entity : _entities) {
			engine::Engine::Instance().DeleteEntity(entity.second->GetId());
		}
		_entities.clear();

		ShaderManager::instance().Delete(_meshShader);

		_meshShader = 0;
	}
};
