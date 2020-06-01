#pragma once

#include <memory>
#include "lazy.hpp"
#include "ui/UI.hpp"
#include "ecs/ecs.hpp"
#include "EngineObject.hpp"
#include "assimp/Model.hpp"
#include "Mesh.hpp"
#include "Batch.hpp"
#include <unordered_map>
#include "TextureManager.hpp"

using namespace lazy;
using namespace graphics;
using namespace inputs;
using namespace utils;

namespace ecs
{
	class SystemManager;
	class EntityManager;
}

namespace engine
{

class Engine
{
private:
	std::unique_ptr<Display> _display;
	std::unique_ptr<Camera> _camera;
	std::unique_ptr<UI> _ui;

	ecs::ECSEngine _ecs;

	ecs::EntityManager *_entityManager;
	ecs::SystemManager *_systemManager;

	std::vector<std::unique_ptr<EngineObject>> _engineObjects;

	std::unordered_map<unsigned int, std::unique_ptr<IDrawable>> _meshes;
	std::unordered_map<unsigned int, std::unique_ptr<Batch>> _batches;

	static unsigned int _nextId;

private:
	Engine();

public:
	Engine(Engine const &) = delete;
	void operator=(Engine const &) = delete;

	static Engine &Instance()
	{
		static Engine engine;
		return engine;
	}

	int Run();
	void Update();
	void UpdateObjects();
	void UpdateSubobjects(std::vector<EngineObject*> subobjects);

	lazy::graphics::Display *GetDisplay() { return _display.get(); }

	template <typename T, typename ... ArgTypes>
	[[nodiscard]] T *CreateEngineObject(ArgTypes... args)
	{
		static_assert(std::is_base_of<EngineObject, T>::value && !std::is_same<EngineObject, T>::value,
			"T must be derived from EngineObject");

		auto object = std::make_unique<T>(&_ecs, std::forward<ArgTypes>(args)...);
		auto ret = object.get();

		_engineObjects.push_back(std::move(object));

		return ret;
	}

	template <typename T, typename ... ArgTypes>
	void CreateComponentSystem(ArgTypes... args)
	{
		static_assert(std::is_base_of<ecs::ComponentSystem, T>::value && !std::is_same<ecs::ComponentSystem, T>::value,
			"T must be derived from EngineObject");

		_ecs.GetSystemManager()->InstantiateSystem<T>(std::forward(args)...);
	}

	template <typename ... ArgTypes>
	[[nodiscard]] ecs::IEntity<ArgTypes...> *CreateEntity()
	{
		return _ecs.GetEntityManager()->CreateEntity<ArgTypes...>();
	}

	std::vector<unsigned int> LoadMeshes(std::string const &path)
	{
		assimp::Model model(path);
		std::vector<unsigned int> ids;

		auto textures = model.getTextures();
		for (auto const &[type, textureType] : textures) {
			for (auto const &texture: textureType) {
				TextureManager::instance().createTexture(texture.name, texture.path, {
					{ GL_TEXTURE_WRAP_S, GL_REPEAT },
					{ GL_TEXTURE_WRAP_T, GL_REPEAT },
					{ GL_TEXTURE_MIN_FILTER, GL_LINEAR },
					{ GL_TEXTURE_MAG_FILTER, GL_LINEAR },
				}, GL_TEXTURE_2D);
			}
		}

		ids.reserve(model.getMeshes().size());
		for (auto &m : model.getMeshes()) {
			auto to_ptr = std::make_unique<Mesh>(std::move(m));

			_meshes[_nextId] = std::move(to_ptr);
			ids.push_back(_nextId);

			_nextId++;
		}

		return ids;
	}

	unsigned int AddMesh(Mesh mesh)
	{
		auto to_ptr = std::make_unique<Mesh>(std::move(mesh));
		_meshes[_nextId] = std::move(to_ptr);

		return _nextId++;
	}

	IDrawable *GetMesh(unsigned int id)
	{
		auto mesh = _meshes.find(id);

		if (mesh != _meshes.end()) {
			return mesh->second.get();
		}

		return nullptr;
	}

	unsigned int AddBatch(std::unique_ptr<Batch> batch)
	{
		_meshes[_nextId] = std::move(batch);

		return _nextId++;
	}

	Batch *GetBatch(unsigned int id)
	{
		auto batch = _batches.find(id);

		if (batch != _batches.end()) {
			return batch->second.get();
		}

		return nullptr;
	}
};

}
