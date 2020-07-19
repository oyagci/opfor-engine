#include "Engine.hpp"
#include "Time.hpp"
#include "utils/Settings.hpp"
#include "stb_image.h"
#include "components/SelectedComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/LuaScriptComponent.hpp"
#include "engine/Model.hpp"
#include "Level.hpp"
#include "engine/platform/opengl/OpenGLContext.hpp"
#include "engine/platform/linux/LinuxInput.hpp"

namespace opfor
{

unsigned int Engine::_nextId = 0;
unsigned int Engine::_nextMaterialId = 0;

Engine::Engine()
{
	_window = IWindow::Create({ "OPFOR - Untitled Project", 1920, 1080 });
	_context = MakeUnique<OpenGLContext>(reinterpret_cast<GLFWwindow*>(_window->GetRawHandle()));
	_context->Init();

	_ui = std::make_unique<UI>(_window->GetWidth(), _window->GetHeight());

	_systemManager = _ecs.GetSystemManager();
	_entityManager = _ecs.GetEntityManager();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	_selectItem = Callback<size_t>([&] (size_t id) {
		auto ents = _ecs.GetEntityManager()->GetAllEntities();
		std::vector<ecs::IEntityBase const *> matches;

		for (auto const &ent : ents) {
			if (ent->GetId() == id) {
				matches.push_back(ent);
			}
		}

		if (matches.size() > 0) {
			auto prev = _ecs.GetEntityManager()->GetEntities<SelectedComponent>();
			for (auto &p : prev) {
				p->DeleteComponents<SelectedComponent>();
			}

			auto entity = _ecs.GetEntityManager()->GetEntity(matches[0]->GetId());
			if (entity.has_value()) {
				entity.value()->AddComponents<SelectedComponent>();
			}
		}
	});
	OnSelectItem += _selectItem;

	_currentLevel = std::make_unique<Level>();
}

/*
 * Needed because unique_ptr needs complete types
 * and there are some forward declared classes in the hpp file
 */
Engine::~Engine() = default;

int Engine::Run()
{
	Settings::Get().load("config.ini");

	while (!_window->IsClosed())
	{
		float deltaTime = Time::Get().getDeltaTime();

		Update();

		_ecs.Update(deltaTime);

		_context->SwapBuffers();

		_ui->update();
		_ui->render();
		Input::Flush();
	}

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
