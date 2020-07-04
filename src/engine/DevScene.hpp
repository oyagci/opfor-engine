#pragma once

#include "Scene.hpp"
#include "Model.hpp"
#include "ShaderManager.hpp"

#include "components/LuaScriptComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/MeshComponent.hpp"

#include "systems/LuaSystem.hpp"
#include "systems/MeshRendererSystem.hpp"
#include "systems/CameraMovementSystem.hpp"
#include "systems/SkyboxRendererSystem.hpp"

class DevScene : public Scene
{
	unsigned int _MeshShader;

	ecs::IEntityBase *_PlayerCamera;
	ecs::IEntityBase *_PointLight;
	ecs::IEntityBase *_Wall;

public:
	DevScene()
	{
		auto [ shaderId, shader ] = ShaderManager::instance().Create();

		shader.addVertexShader("shaders/basic.vs.glsl")
			.addFragmentShader("shaders/basic.fs.glsl")
			.link();

		shader.bind();
			shader.setUniform1i("material.albedo", 0);
			shader.setUniform1i("material.metallicRoughness", 1);
			shader.setUniform1i("material.normal", 2);
		shader.unbind();

		_MeshShader = shaderId;
	}

	void Setup() override
	{
		ECS().SystemManager->InstantiateSystem<MeshRendererSystem>();
		ECS().SystemManager->InstantiateSystem<LuaSystem>();
		ECS().SystemManager->InstantiateSystem<CameraMovementSystem>();

		// Point Light
		// ===========

		_PointLight = ECS().EntityManager->CreateEntity<PointLightComponent, TransformComponent>();
		auto &lightProps = _PointLight->Get<PointLightComponent>();
			lightProps.Color = { 1.0f, 1.0f, 0.8f };
			lightProps.Intensity = 5000.0f;
		_PointLight->Get<TransformComponent>().position = { 100.0f, 100.0f, 100.0f };
		_PointLight->SetName("Point Light");

		// Camera
		// ======

		_PlayerCamera = ECS().EntityManager->CreateEntity<PlayerCameraComponent, TransformComponent>();
		auto &camera = _PlayerCamera->Get<PlayerCameraComponent>();
			camera.projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
			camera.model = glm::mat4(1.0f);
			camera.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			camera.viewProjection = camera.projection * camera.view;
			camera.exposure = 2.0f;
		_PlayerCamera->Get<TransformComponent>().position = { 0.0f, 0.0f, 0.0f };
		_PlayerCamera->SetName("Editor Camera");

		// Wall
		// ====

		engine::Model engineModel;
		engineModel.LoadFromGLTF("models/42Run/MapTiles/Wall-1.gltf");

		_Wall = ECS().EntityManager->CreateEntity<ModelComponent, TransformComponent>();

		auto &trans = _Wall->Get<TransformComponent>();
			trans.position = { 0.0f, 0.0f, 0.0f };
			trans.scale = { 1.0f, 1.0f, 1.0f };

		auto model = _Wall->Get<ModelComponent>();
		auto &meshes = engineModel.GetMeshes();
			model.Meshes.insert(model.Meshes.begin(), meshes.begin(), meshes.end());
			model.Path = "models/42Run/MapTiles/Wall-1.gltf";
			model.Shader = _MeshShader;

		_Wall->Set(model);
		_Wall->SetName("Wall");
	}

	void Play() override
	{
		_PointLight->AddComponents<LuaScriptComponent>();
		auto &lightScript = _PointLight->Get<LuaScriptComponent>();
			lightScript.Path = "scripts/custom_script.lua";
			lightScript.Runtime.Load("scripts/custom_script.lua");
	}
};
