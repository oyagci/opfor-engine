#pragma once

#include <unordered_map>

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

	using TileEnt = ecs::IEntity<ModelComponent, TransformComponent>*;
	using TileRow = std::vector<TileEnt>;

	std::vector<TileRow> _levelRows;
	std::vector<TileEnt> _unusedTiles;

	float _levelOffset = 0.0f;

	engine::Model _DoorModel;
	engine::Model _WallModel;
	engine::Model _FloorModel;
	engine::Model _PillarModel;
	engine::Model _DeskModel;
	engine::Model _WindowLeft;
	engine::Model _Ceiling;
	engine::Model _WindowLeftCeiling;
	engine::Model _WallCeiling;

	enum class Tile {
		Floor,
		Desk,
		Wall,
		Door,
		WindowLeft,
		Ceiling,
		WindowLeftCeiling,
		WallCeiling,
	};

	std::optional<ecs::IEntity<ModelComponent, TransformComponent>*> MakeTile(Tile type,
		glm::vec3 position = { 0.0f, 0.0f, 0.0f }, glm::vec3 scale = { 1.0f, 1.0f, 1.0f })
	{
		std::optional<ecs::IEntity<ModelComponent, TransformComponent>*> newTile;
		std::unordered_map<Tile, engine::Model&> const tileList = {
			{ Tile::Floor, _FloorModel },
			{ Tile::Desk,  _DeskModel },
			{ Tile::Wall,  _WallModel },
			{ Tile::Door,  _DoorModel },
			{ Tile::WindowLeft, _WindowLeft },
			{ Tile::Ceiling, _Ceiling },
			{ Tile::WindowLeftCeiling, _WindowLeftCeiling },
			{ Tile::WallCeiling, _WallCeiling },
		};

		auto tileInfo = tileList.find(type);
		if (tileInfo != tileList.end()) {

			auto const &model = tileInfo->second;
			auto tile = ECS().EntityManager->CreateEntity<ModelComponent, TransformComponent>();

			auto &trans = tile->Get<TransformComponent>();
				trans.position = position;
				trans.scale = scale;

			auto modelComp = tile->Get<ModelComponent>();

			auto const &meshes = model.GetMeshes();
				modelComp.Meshes.insert(modelComp.Meshes.begin(), meshes.begin(), meshes.end());
				modelComp.Shader = _MeshShader;

			tile->Set(modelComp);
			tile->SetName("Wall");

			newTile = tile;
		}

		return newTile;
	}

	void LoadModels()
	{
		_DoorModel.LoadFromGLTF("models/42Run/MapTiles/Cluster-Door.gltf");
		_WallModel.LoadFromGLTF("models/42Run/MapTiles/Cluster-Wall.gltf");
		_FloorModel.LoadFromGLTF("models/42Run/MapTiles/Cluster-Floor.gltf");
		_DeskModel.LoadFromGLTF("models/42Run/MapTiles/Cluster-Desk.gltf");
		_PillarModel.LoadFromGLTF("models/42Run/MapTiles/Cluster-Pillar.gltf");
		_WindowLeft.LoadFromGLTF("models/42Run/MapTiles/Cluster-Window.gltf");
		_Ceiling.LoadFromGLTF("models/42Run/MapTiles/Cluster-Ceiling.gltf");
		_WindowLeftCeiling.LoadFromGLTF("models/42Run/MapTiles/Cluster-Window-Ceiling.gltf");
		_WallCeiling.LoadFromGLTF("models/42Run/MapTiles/Cluster-Door-Ceiling.gltf");
	}

	void SetupLevel()
	{
		LoadModels();

		// Point Light
		// ===========
		_PointLight = ECS().EntityManager->CreateEntity<PointLightComponent, TransformComponent>();
		auto &lightProps = _PointLight->Get<PointLightComponent>();
			lightProps.Color = { 1.0f, 1.0f, 0.8f };
			lightProps.Intensity = 100000.0f;
		_PointLight->Get<TransformComponent>().position = { 0.0f, 220.0f, 0.0f };
		_PointLight->SetName("Point Light");

		// Camera
		// ======
		_PlayerCamera = ECS().EntityManager->CreateEntity<PlayerCameraComponent, TransformComponent>();
		auto &camera = _PlayerCamera->Get<PlayerCameraComponent>();
			camera.projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 5000.0f);
			camera.model = glm::mat4(1.0f);
			camera.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			camera.viewProjection = camera.projection * camera.view;
			camera.exposure = 2.0f;
		_PlayerCamera->Get<TransformComponent>().position = { 0.0f, 0.0f, 0.0f };
		auto &camTrans = _PlayerCamera->Get<TransformComponent>();
			camTrans.position = { 0.0f, 150.0f, 0.0f };
			camTrans.yaw = 0.0f;
			camTrans.pitch = 0.0f;
		_PlayerCamera->SetName("Editor Camera");

		// Wall
		// ====

		for (int i = 0; i < 10; i++) {

			std::vector<TileEnt> row;

			row.push_back(MakeTile(Tile::WindowLeftCeiling, { 200.0f * i, 0.0f, -400.0f }).value());
			row.push_back(MakeTile(Tile::WindowLeft, { 200.0f * i, 0.0f, -400.0f }).value());

			row.push_back(MakeTile(Tile::WindowLeft, { 200.0f * i, 0.0f, 400.0f }, { -1.0f, 1.0f, -1.0f }).value());
			row.push_back(MakeTile(Tile::WindowLeftCeiling, { 200.0f * i, 0.0f, 400.0f }, { -1.0f, 1.0f, -1.0f }).value());

			if (i % 4 == 0) {
				row.push_back(MakeTile(Tile::WallCeiling, { 200.0f * i, 200.0f, 0.0f }).value());
				row.push_back(MakeTile(Tile::Door, { 200.0f * i, 0.0f, 0.0f }).value());
				row.push_back(MakeTile(Tile::Floor, { 200.0f * i, 0.0f, 0.0f }).value());

				row.push_back(MakeTile(Tile::WallCeiling, { 200.0f * i, 200.0f, 200.0f }).value());
				row.push_back(MakeTile(Tile::Wall, { 200.0f * i, 0.0f, 200.0f }).value());
				row.push_back(MakeTile(Tile::Floor, { 200.0f * i, 0.0f, 200.0f }).value());

				row.push_back(MakeTile(Tile::WallCeiling, { 200.0f * i, 200.0f, -200.0f }).value());
				row.push_back(MakeTile(Tile::Wall, { 200.0f * i, 0.0f, -200.0f }).value());
				row.push_back(MakeTile(Tile::Floor, { 200.0f * i, 0.0f, -200.0f }).value());
			}
			else {
				row.push_back(MakeTile(Tile::Ceiling, { 200.0f * i, 200.0f, 0.0f }).value());
				row.push_back(MakeTile(Tile::Floor, { 200.0f * i, 0.0f, 0.0f }).value());

				row.push_back(MakeTile(Tile::Ceiling, { 200.0f * i, 200.0f, 200.0f }).value());
				row.push_back(MakeTile(Tile::Desk, { 200.0f * i, 0.0f, 200.0f }).value());
				row.push_back(MakeTile(Tile::Floor, { 200.0f * i, 0.0f, 200.0f }).value());

				row.push_back(MakeTile(Tile::Ceiling, { 200.0f * i, 200.0f, -200.0f }).value());
				row.push_back(MakeTile(Tile::Desk, { 200.0f * i, 0.0f, -200.0f }).value());
				row.push_back(MakeTile(Tile::Floor, { 200.0f * i, 0.0f, -200.0f }).value());
			}

			_levelRows.push_back(row);
		}
	}

	void UpdateTiles(float deltaTime)
	{
		std::vector<TileRow> updatedRowList;

		updatedRowList.reserve(_levelRows.size());

		for (auto const &row: _levelRows) {

			bool outOfView = false;

			for (auto const &tile : row) {

				auto &tr = tile->Get<TransformComponent>();
				tr.position.x = tr.position.x - 100.0f * deltaTime;

				// If the tile moved out of the world
				// mark the row as out of view.
				if (tr.position.x <= -200.0f) {
					tr.position.y = -400.0f;
					outOfView = true;
				}
			}

			if (!outOfView) {
				updatedRowList.push_back(row);
			}
			else {
				// Save unused tile for later reuse
				for (auto const &tile : row) {
					_unusedTiles.push_back(tile);
				}
			}
		}

		_levelRows = updatedRowList;
	}

	void GenerateNewTiles()
	{
//		if (_levelTiles.size() >= 10) { return ; }
//
//		float newTileOffset = 200.0f * _levelTiles.size();
//
//		_levelTiles.push_back(MakeTile(Tile::Ceiling, { 200.0f * newTileOffset, 200.0f, 0.0f }).value());
//		_levelTiles.push_back(MakeTile(Tile::Floor, { 200.0f * newTileOffset, 0.0f, 0.0f }).value());
//
//		_levelTiles.push_back(MakeTile(Tile::Ceiling, { 200.0f * newTileOffset, 200.0f, 200.0f }).value());
//		_levelTiles.push_back(MakeTile(Tile::Desk, { 200.0f * newTileOffset, 0.0f, 200.0f }).value());
//		_levelTiles.push_back(MakeTile(Tile::Floor, { 200.0f * newTileOffset, 0.0f, 200.0f }).value());
//
//		_levelTiles.push_back(MakeTile(Tile::Ceiling, { 200.0f * newTileOffset, 200.0f, -200.0f }).value());
//		_levelTiles.push_back(MakeTile(Tile::Desk, { 200.0f * newTileOffset, 0.0f, -200.0f }).value());
//		_levelTiles.push_back(MakeTile(Tile::Floor, { 200.0f * newTileOffset, 0.0f, -200.0f }).value());
	}

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

		SetupLevel();
	}

	void Play() override
	{
		_PointLight->AddComponents<LuaScriptComponent>();
		auto &lightScript = _PointLight->Get<LuaScriptComponent>();
			lightScript.Path = "scripts/custom_script.lua";
			lightScript.Runtime.Load("scripts/custom_script.lua");
	}

	void Update(float deltaTime) override
	{
		auto &lightTrans = _PointLight->Get<TransformComponent>();
		auto &playerTrans = _PlayerCamera->Get<TransformComponent>();

		lightTrans.position.x = playerTrans.position.x;

		_levelOffset += 300.0f * deltaTime;

		UpdateTiles(deltaTime);
		GenerateNewTiles();
	}
};
