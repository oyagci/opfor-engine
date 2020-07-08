#include "Engine.hpp"
#include "Time.hpp"
#include "utils/Settings.hpp"
#include "stb_image.h"
#include "components/SelectedComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/LuaScriptComponent.hpp"
#include "engine/Model.hpp"
#include "systems/ImguiSystem.hpp"
#include "DevScene.hpp"

namespace engine
{

unsigned int Engine::_nextId = 0;
unsigned int Engine::_nextMaterialId = 0;

Engine::Engine()
{
	_display = std::make_unique<lazy::graphics::Display>("3D Engine", 1920, 1080);
	_display->enableCap(GL_DEPTH_TEST);
	_display->enableCap(GL_CULL_FACE);
	_display->enableCap(GL_BLEND);
	_display->setFullscreen(false);
	_display->showCursor(false);
	glEnable(GL_DEBUG_OUTPUT);

	lazy::maths::transform t = { glm::vec3(32, 64, 32), glm::quat(), glm::vec3(1), nullptr };
	_camera = std::make_unique<lazy::graphics::Camera>(*_display, t);
	_camera->setProjection(glm::radians(80.0f), 0.1f, 1000.0f);

	_ui = std::make_unique<UI>(_display->getWidth(), _display->getHeight());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	_ecs = ecs::ECSEngine::Get().CreateInstance();

	_selectItem = Callback<size_t>([&] (size_t id) {
		auto ents = _ecs->EntityManager->GetAllEntities();
		std::vector<ecs::IEntityBase const *> matches;

		for (auto const &ent : ents) {
			if (ent->GetId() == id) {
				matches.push_back(ent);
			}
		}

		if (matches.size() > 0) {
			auto prev = _ecs->EntityManager->GetEntities<SelectedComponent>();
			for (auto &p : prev) {
				p->DeleteComponents<SelectedComponent>();
			}

			auto entity = _ecs->EntityManager->GetEntity(matches[0]->GetId());
			if (entity.has_value()) {
				entity.value()->AddComponents<SelectedComponent>();
			}
		}
	});
	OnSelectItem += _selectItem;

	TextureManager::instance().createTexture("prototype_tile_8", "./img/prototype_tile_8.png", {
		{ GL_TEXTURE_MAG_FILTER, GL_LINEAR },
		{ GL_TEXTURE_MIN_FILTER, GL_LINEAR },
		{ GL_TEXTURE_WRAP_R, GL_REPEAT },
		{ GL_TEXTURE_WRAP_S, GL_REPEAT },
	}, GL_TEXTURE_2D);

	TextureManager::instance().createTexture("default_normal", "./img/default_normal.png", {
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
		{ GL_TEXTURE_WRAP_R, GL_REPEAT },
		{ GL_TEXTURE_WRAP_S, GL_REPEAT },
	}, GL_TEXTURE_2D);
}

/*
 * Needed because unique_ptr needs complete types
 * and there are some forward declared classes in the hpp file
 */
Engine::~Engine() = default;

int Engine::Run()
{
	Settings::instance().load("config.ini");

	_Scene = std::make_unique<DevScene>();
	_Scene->OnSetup();
	_Scene->OnPlay();
	while (!_display->isClosed())
	{
		float deltaTime = Time::instance().getDeltaTime();

		Update();
		_Scene->OnUpdate(deltaTime);

		_camera->update();

		ecs::ECSEngine::Get().Update(deltaTime);

		_ui->update();
		_ui->render();

		_display->update();
		_display->updateInputs();
	}
	_Scene->OnStop();

	return 0;
}

void Engine::Update()
{
	UpdateObjects();
}

void Engine::UpdateObjects()
{
	for (auto &o : _engineObjects) {
		o->Update();
		UpdateSubobjects(o->GetSubobjects());
	}
}


void Engine::UpdateSubobjects(std::vector<EngineObject*> subobjects)
{
	for (auto &o : subobjects) {
		o->Update();
		UpdateSubobjects(o->GetSubobjects());
	}
}

unsigned int Engine::RegisterModel(Model model)
{
	auto modelPtr = std::make_unique<Model>(model);

	_models[_nextId] = std::move(modelPtr);
	return _nextId++;
}

void Engine::RemoveModel(unsigned int id)
{
	auto model = _models.find(id);
	if (model != _models.end()) {
		_models.erase(model);
	}
}

std::optional<Model const *> Engine::GetModel(unsigned int id) const
{
	auto const model = _models.find(id);

	if (model != _models.end()) {
		return std::make_optional(model->second.get());
	}

	return std::nullopt;
}

void Engine::OnRebuildModel(ModelComponent &model)
{
	for (auto const &m : model.Meshes) {
		RemoveModel(m);
	}
	model.Meshes.clear();

	Model modelData{};
	modelData.LoadFromGLTF(model.Path);

	auto const &meshes = modelData.GetMeshes();

	if (meshes.size() > 0) {
		model.Meshes.reserve(meshes.size());
		model.Meshes.insert(model.Meshes.begin(), meshes.begin(), meshes.end());
	}
}

void Engine::OnReloadScript(LuaScriptComponent &script)
{
	script.Runtime.Reset();
	script.Runtime.Load(script.Path);
}

}
