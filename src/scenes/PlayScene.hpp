#pragma once

#include <unordered_map>

#include "Scene.hpp"
#include "Model.hpp"
#include "ShaderManager.hpp"

#include "components/LuaScriptComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/Run42Player.hpp"
#include "components/BoxColliderComponent.hpp"
#include "components/TextComponent.hpp"

#include "systems/LuaSystem.hpp"
#include "systems/MeshRendererSystem.hpp"
#include "systems/CameraMovementSystem.hpp"
#include "systems/SkyboxRendererSystem.hpp"
#include "systems/Run42PlayerSystem.hpp"
#include "systems/Collision3DSystem.hpp"
#include "systems/TextRendererSystem.hpp"

#include <random>

class PlayScene : public Scene
{
	enum class SceneState {
		Stopped,
		Playing,
		Paused,
	} _State = SceneState::Stopped;

	unsigned int _MeshShader;

	ecs::IEntityBase *_PlayerCamera;
	ecs::IEntityBase *_PointLight;

	using PlayerEnt = ecs::IEntity<ModelComponent, TransformComponent, Run42PlayerComponent, BoxCollider3DComponent>;

	PlayerEnt *_Player;

	using TileEnt = ecs::IEntity<ModelComponent, TransformComponent, BoxCollider3DComponent>*;
	using TileRow = std::vector<TileEnt>;

	std::vector<TileRow> _levelRows;
	std::vector<TileEnt> _unusedTiles;

	float _moveSpeed = 200.0f;

	float _levelOffset = 0.0f;
	float _newRowOffset = 0.0f;

	float _distanceTraveled = 0;
	size_t _highScore = 0.0f;

	unsigned int _totalNumberOfRows = 0;

	// Random value generator for the level generator
	std::random_device _RandomDevice;
	std::default_random_engine _RandomEngine;

	engine::Model _DoorModel;
	engine::Model _WallModel;
	engine::Model _FloorModel;
	engine::Model _PillarModel;
	engine::Model _DeskModel;
	engine::Model _WindowLeft;
	engine::Model _WindowLeftCeiling;
	engine::Model _Empty;
	engine::Model _Transhcans;
	engine::Model _Marvin;

	ecs::IEntity<TextComponent> *_ScoreText;

	enum class Tile {
		None,
		Floor,
		Desk,
		Wall,
		Door,
		WindowLeft,
		WindowLeftCeiling,
		Pillar,
		Trashcans,
	};

	static constexpr char const *formatStr = "Score: {} | Speed: {:.2} m/s | High Score {}";

	std::optional<TileEnt> MakeTile(Tile type,
		glm::vec3 position = { 0.0f, 0.0f, 0.0f }, glm::vec3 scale = { 1.0f, 1.0f, 1.0f })
	{
		std::optional<TileEnt> newTile;
		std::unordered_map<Tile, std::tuple<engine::Model&, std::optional<BoxCollider3DComponent>>> const tileList = {
            // Tile Type               // 3D Model           // Optional 2D Collider (x, y, w, h)
			{ Tile::Floor,             { _FloorModel,        std::nullopt } },
			{ Tile::Desk,              { _DeskModel,         BoxCollider3DComponent::New(glm::vec3(-30.0f, 0.0f, -100.0f), glm::vec3(60.0f, 200.0f, 200.0f)) } },
			{ Tile::Wall,              { _WallModel,         BoxCollider3DComponent::New(glm::vec3(-10.0f, 0.0f, -100.0f), glm::vec3(20.0f, 200.0f, 200.0f)) } },
			{ Tile::Door,              { _DoorModel,         std::nullopt } },
			{ Tile::WindowLeft,        { _WindowLeft,        std::nullopt } },
			{ Tile::WindowLeftCeiling, { _WindowLeftCeiling, std::nullopt } },
			{ Tile::Pillar,            { _PillarModel,       BoxCollider3DComponent::New(glm::vec3(-40.0f, 0.0f, -40.0f), glm::vec3(80.0f, 200.0f, 80.0f)) } },
			{ Tile::Trashcans,         { _Transhcans,        BoxCollider3DComponent::New(glm::vec3(-30.0f, 0.0f, -30.0f), glm::vec3(60.0f, 30.0f,  60.0f)) } },
			{ Tile::None,	           { _Empty,             std::nullopt } },
		};

		auto tileInfo = tileList.find(type);
		if (tileInfo != tileList.end()) {

			auto const &model = std::get<0>(tileInfo->second);

			TileEnt tile = nullptr;

			// Recycle existing unused tiles
			// TODO: Do it in the ECS directly
			if (!_unusedTiles.empty()) {
				tile = _unusedTiles.back();
				_unusedTiles.pop_back();
			}
			else {
				tile = ECS().EntityManager->CreateEntity<ModelComponent, TransformComponent, BoxCollider3DComponent>();
			}

			auto &trans = tile->Get<TransformComponent>();
				trans.position = position;
				trans.scale = scale;

			ModelComponent modelComp{};

			auto const &meshes = model.GetMeshes();
				modelComp.Meshes.insert(modelComp.Meshes.begin(), meshes.begin(), meshes.end());
				modelComp.Shader = _MeshShader;

			if (tile->HasComponents<BoxCollider3DComponent>()) {
				tile->DeleteComponents<BoxCollider3DComponent>();
			}

			if (std::get<1>(tileInfo->second).has_value()) {
				tile->AddComponents<BoxCollider3DComponent>();
				auto collider = std::get<1>(tileInfo->second).value();
				tile->Set(collider);
			}

			tile->Set(modelComp);

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
		_WindowLeftCeiling.LoadFromGLTF("models/42Run/MapTiles/Cluster-Window-Ceiling.gltf");
		_Empty.LoadFromGLTF("models/42Run/MapTiles/Cluster-Empty.gltf");
		_Transhcans.LoadFromGLTF("models/42Run/MapTiles/Cluster-Trashcans.gltf");
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
			camera = PlayerCameraComponent::New();
		auto &camTrans = _PlayerCamera->Get<TransformComponent>();
			camTrans = TransformComponent::New();
			camTrans.position = { -180.0f, 150.0f, 0.0f };

		_Marvin.LoadFromGLTF("models/42Run/Marvin/scene.gltf");

		_Player = ECS().EntityManager->CreateEntity<ModelComponent, TransformComponent, Run42PlayerComponent, BoxCollider3DComponent>();
		auto &model = _Player->Get<ModelComponent>();
			model.Meshes.reserve(_Marvin.GetMeshes().size());
			model.Meshes.insert(model.Meshes.begin(), _Marvin.GetMeshes().begin(), _Marvin.GetMeshes().end());
		auto &playerTrans = _Player->Get<TransformComponent>();
			playerTrans.position = { 0.0f, 0.0f, 0.0f };
			playerTrans.scale = { 0.1f, 0.1f, 0.1f };

		_Player->Set(BoxCollider3DComponent::New({ -10.0f, 0.0f, -10.0f }, { 20.0f, 100.0f, 20.0f }));

		for (int i = 0; i < 10; i++) {
			GenerateRow();
		}
	}

	void UpdateTiles(float deltaTime)
	{
		std::vector<TileRow> updatedRowList;

		float speed = _moveSpeed;
		float dist = speed * deltaTime;

		updatedRowList.reserve(_levelRows.size());

		_newRowOffset -= dist;

		for (auto const &row: _levelRows) {

			bool outOfView = false;

			for (auto const &tile : row) {

				auto &tr = tile->Get<TransformComponent>();
				tr.position.x = tr.position.x - dist;

				// If the tile moved out of the world
				// mark the row as out of view.
				if (tr.position.x <= -200.0f) {
					tr.position.x = -200.0f;
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

	void GenerateRow()
	{
		// List of all the possible row layouts
		static const std::vector<std::array<Tile, 3>> layouts = {
			  // Left          // Middle        // Right
			{ Tile::None,      Tile::Desk,      Tile::Desk },
			{ Tile::Desk,      Tile::None,      Tile::Desk },
			{ Tile::Desk,      Tile::Desk,      Tile::None },

			{ Tile::Wall,      Tile::Door,      Tile::Wall },

			{ Tile::Pillar,    Tile::None,      Tile::Pillar },
			{ Tile::Pillar,    Tile::Desk,      Tile::None },
			{ Tile::Pillar,    Tile::None,      Tile::Desk },

			{ Tile::Desk,      Tile::None,      Tile::Pillar },
			{ Tile::None,      Tile::Desk,      Tile::Pillar },

			{ Tile::None,      Tile::Desk,      Tile::None },
			{ Tile::None,      Tile::None,      Tile::Pillar },

			{ Tile::Trashcans, Tile::Desk,      Tile::Desk },
			{ Tile::Desk,      Tile::Trashcans, Tile::Desk },
			{ Tile::Desk,      Tile::Desk,      Tile::Trashcans },
		};

		static constexpr std::array<Tile, 3> startRow = { Tile::None, Tile::None, Tile::None };

		// Guard: Maximum number of rows present at the same time
		if (_levelRows.size() >= 10) { return ; }

		std::vector<TileEnt> row;

		// The end of the level is at this offset
		glm::vec3 newRowOff = { _newRowOffset, 0.0f, 0.0f };

		// Left Window
		row.push_back(MakeTile(Tile::WindowLeftCeiling, glm::vec3(0.0f, 0.0f, -400.0f) + newRowOff).value());
		row.push_back(MakeTile(Tile::WindowLeft,		glm::vec3(0.0f, 0.0f, -400.0f) + newRowOff).value());

		// Right Window
		row.push_back(MakeTile(Tile::WindowLeftCeiling, glm::vec3(0.0f, 0.0f,  400.0f) + newRowOff, { -1.0f, 1.0f, -1.0f }).value());
		row.push_back(MakeTile(Tile::WindowLeft,		glm::vec3(0.0f, 0.0f,  400.0f) + newRowOff, { -1.0f, 1.0f, -1.0f }).value());

		if (_totalNumberOfRows < 3) {

			_totalNumberOfRows += 1;

			row.push_back(MakeTile(Tile::Floor, glm::vec3(0.0f, 000.0f,  200.0f) + newRowOff).value());
			row.push_back(MakeTile(startRow[0], glm::vec3(0.0f, 000.0f,  200.0f) + newRowOff).value());

			row.push_back(MakeTile(Tile::Floor, glm::vec3(0.0f, 000.0f,  000.0f) + newRowOff).value());
			row.push_back(MakeTile(startRow[1], glm::vec3(0.0f, 000.0f,  000.0f) + newRowOff).value());

			row.push_back(MakeTile(Tile::Floor, glm::vec3(0.0f, 000.0f, -200.0f) + newRowOff).value());
			row.push_back(MakeTile(startRow[2], glm::vec3(0.0f, 000.0f, -200.0f) + newRowOff).value());
		}
		else {

			// Choose a row layout randomly
			std::uniform_int_distribution<unsigned int>	random(0, layouts.size() - 1);
			unsigned int randomIndex = random(_RandomEngine);

			auto layout = layouts[randomIndex];

			// Left Row
			row.push_back(MakeTile(Tile::Floor, glm::vec3(0.0f, 000.0f,  200.0f) + newRowOff).value());
			row.push_back(MakeTile(layout[0],   glm::vec3(0.0f, 000.0f,  200.0f) + newRowOff).value());

			// Middle Row
			row.push_back(MakeTile(Tile::Floor, glm::vec3(0.0f, 000.0f,  000.0f) + newRowOff).value());
			row.push_back(MakeTile(layout[1],   glm::vec3(0.0f, 000.0f,  000.0f) + newRowOff).value());

			// Right Row
			row.push_back(MakeTile(Tile::Floor, glm::vec3(0.0f, 000.0f, -200.0f) + newRowOff).value());
			row.push_back(MakeTile(layout[2],   glm::vec3(0.0f, 000.0f, -200.0f) + newRowOff).value());
		}

		_levelRows.push_back(row);

		// Since we've added a new row the end of the level is further away now
		_newRowOffset += 200.0f;
	}

	void CheckCollision()
	{
		auto const &collider = _Player->Get<BoxCollider3DComponent>();
		static int index = 0;

		if (collider.IsColliding == true) {
			fmt::print("[{}] Player Is Colliding!\n", index++);
			_State = SceneState::Stopped;
			if (_distanceTraveled > _highScore) {
				_highScore = _distanceTraveled / 100;
			}
			ResetLevel();
		}
	}

	void ResetLevel()
	{
		for (auto &row : _levelRows) {
			for (auto &tile : row) {
				_unusedTiles.push_back(tile);
			}
			row.clear();
		}
		_levelRows.clear();

		_distanceTraveled = 0.0f;
		_levelOffset = 0.0f;
		_newRowOffset = 0.0f;
		_totalNumberOfRows = 0;
		_moveSpeed = 200.0f;

		for (int i = 0; i < 10; i++) {
			GenerateRow();
		}

		_State = SceneState::Stopped;
	}

public:
	PlayScene()
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

		_RandomEngine = std::default_random_engine(_RandomDevice());
	}

	void Setup() override
	{
		ECS().SystemManager->InstantiateSystem<TextRendererSystem>();
		ECS().SystemManager->InstantiateSystem<MeshRendererSystem>();
		ECS().SystemManager->InstantiateSystem<LuaSystem>();
		ECS().SystemManager->InstantiateSystem<CameraMovementSystem>();
		ECS().SystemManager->InstantiateSystem<Run42PlayerSystem>();
		ECS().SystemManager->InstantiateSystem<Collision3DSystem>();

		_ScoreText = ECS().EntityManager->CreateEntity<TextComponent>();

		auto text = TextComponent::New(fmt::format(formatStr, static_cast<size_t>(_distanceTraveled / 100), _moveSpeed / 100, _highScore));

		_ScoreText->Set(text);

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

		auto &kbd = lazy::inputs::input::getKeyboard();

		// TODO: FSM!
		if (_State == SceneState::Playing) {

			UpdateTiles(deltaTime);
			GenerateRow();
			CheckCollision();

			_moveSpeed += 1.0f * deltaTime;
			_distanceTraveled += 300.0f * deltaTime;

			auto text = TextComponent::New(fmt::format(formatStr, static_cast<size_t>(_distanceTraveled / 100), _moveSpeed / 100, _highScore));

			_ScoreText->Set(text);

			if (kbd.getKeyDown(GLFW_KEY_ESCAPE)) {
				_State = SceneState::Paused;
			}
		}
		else if (_State == SceneState::Stopped) {

			if (kbd.getKeyDown(GLFW_KEY_ENTER)) {
				_State = SceneState::Playing;
			}
		}
		else if (_State == SceneState::Paused) {

			if (kbd.getKeyDown(GLFW_KEY_ESCAPE)) {
				_State = SceneState::Playing;
			}
		}
	}
};
