#pragma once

#include "Engine.hpp"
#include "Level.hpp"
#include "ShaderManager.hpp"
#include "Model.hpp"

#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/DisplayComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/LuaScriptComponent.hpp"

// TODO: Something better
MeshComponent initSkybox();

class MainLevel : public engine::ILevel
{
private:
	unsigned int _meshShader;
	unsigned int _skyboxShader;

	ecs::IEntityBase *_player;
	ecs::IEntityBase *_display;
	ecs::IEntityBase *_skybox;
	ecs::IEntityBase *_pointLight;

	unsigned int _sponza;
	unsigned int _pbrSphere;
	unsigned int _env;

public:
	void Load() override
	{
		auto &engine = engine::Engine::Instance();

		_player = engine.CreateEntity<PlayerCameraComponent, TransformComponent>();

		PlayerCameraComponent p{};
			p.projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
			p.model = glm::mat4(1.0f);
			p.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			p.viewProjection = p.projection * p.view;
			p.exposure = 2.0f;
		_player->Set(p);
		_player->SetName("Player");

		TransformComponent t;
			t.direction = glm::vec3(0.0f, 0.0f, 1.0f);
			t.position = glm::vec3(0.0f, 18.0f, -0.5f);
		_player->Set(t);

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

		_display = engine.CreateEntity<DisplayComponent>();
		DisplayComponent d;
			d.display = engine.GetDisplay();
		_display->Set(d);
		_display->SetName("Display");

		auto [ skyboxShaderId, skyboxShader ] = ShaderManager::instance().Create();
		skyboxShader.addVertexShader("./shaders/cubemap.vs.glsl")
			.addFragmentShader("./shaders/cubemap.fs.glsl")
			.link();
		skyboxShader.bind();
		skyboxShader.setUniform1i("cubemap", 0);
		skyboxShader.unbind();

		_skyboxShader = skyboxShaderId;

		_skybox = engine.CreateEntity<MeshComponent, TransformComponent, SkyboxComponent>();
		MeshComponent s = initSkybox();
			s.Shader = skyboxShaderId;
		_skybox->Set(s);
		_skybox->SetName("Skybox");

		auto sponModel = engine::Model();
		sponModel.LoadFromGLTF("models/glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf");
		_sponza = engine.RegisterModel(sponModel);

		if (sponModel.GetMeshes().size() > 0) {

			ModelComponent sponzaModel;
				sponzaModel.Name = "Sponza";
				sponzaModel.Shader = shaderId;
				sponzaModel.Meshes.reserve(sponModel.GetMeshes().size());
				sponzaModel.Meshes.insert(sponzaModel.Meshes.begin(), sponModel.GetMeshes().begin(), sponModel.GetMeshes().end());


			TransformComponent sponzaTransform;
			sponzaTransform.scale = { 0.1f, 0.1f, 0.1f };

			auto sponza = engine.CreateEntity<ModelComponent, TransformComponent>();
			sponza->Set(sponzaModel);
			sponza->Set(sponzaTransform);
			sponza->SetName("Sponza");

		}

		auto pbrSphere = engine::Model();
		pbrSphere.LoadFromGLTF("models/PbrSphere/PbrSphere.gltf");
		_pbrSphere = engine.RegisterModel(pbrSphere);

		if (pbrSphere.GetMeshes().size() > 0) {
			for (size_t x = 0; x < 6; x++) {
				for (int y = 0; y < 6; y++) {

					ModelComponent pbrSphereModel{};
						pbrSphereModel.Meshes.reserve(pbrSphere.GetMeshes().size());
						pbrSphereModel.Meshes.insert(pbrSphereModel.Meshes.begin(), pbrSphere.GetMeshes().begin(), pbrSphere.GetMeshes().end());
						pbrSphereModel.Name = "PBR Sphere";
						pbrSphereModel.Shader = shaderId;

					TransformComponent t{};
						t.scale = { 0.05f, 0.05f, 0.05f };
						t.position = { x * 6.0f, (y + 1) * 6.0f, -13.0f };

					auto sphere = engine.CreateEntity<ModelComponent, TransformComponent>();

					sphere->Set(pbrSphereModel);
					sphere->Set(t);
					sphere->SetName(fmt::format("PBR Sphere {}", 5 * x + y));

				}
			}
		}

		auto env = engine::Model();
		env.LoadFromGLTF("models/glTF-Sample-Models/2.0/EnvironmentTest/glTF/EnvironmentTest.gltf");
		_env = engine.RegisterModel(env);

		if (env.GetMeshes().size() > 0) {
			for (auto const &meshId : env.GetMeshes()) {
				auto spon = engine.CreateEntity<MeshComponent, TransformComponent>();
				MeshComponent dvaMesh{};
					dvaMesh.Id = meshId;
					dvaMesh.Shader = shaderId;
				spon->Set(dvaMesh);

				TransformComponent t{};
					t.scale = { 0.10f, 0.10f, 0.10f };
					t.scale = { 1.0f, 1.0f, 1.0f };
				spon->Set(t);

				spon->SetName("EnvTest");
			}
		}

		_pointLight = engine.CreateEntity<PointLightComponent, TransformComponent, SelectedComponent, LuaScriptComponent>();
		PointLightComponent pl;
			pl.Color = glm::vec3(1.0f, 1.0f, 0.8f);
			pl.Dir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
			pl.Intensity = 1000.0f;
		_pointLight->Set(pl);

		auto &pointLightLua = _pointLight->Get<LuaScriptComponent>();
			pointLightLua.Runtime.PushGlobal("__ENTITY_ID__", _pointLight->GetId());
			pointLightLua.Runtime.Load("scripts/custom_script.lua");

		TransformComponent pt;
			pt.position = glm::vec3(-50.0f, 10.0f, 0.0f);
		_pointLight->Set(pt);

		_pointLight->SetName("Point Light");

	}

	void Unload() override
	{
		// TODO
	}
};
