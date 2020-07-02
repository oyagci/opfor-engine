#pragma once

#include "Entity.hpp"

namespace ecs {

class EntityManager;

class EntityManager;
class SystemManager;

class ECSEngine
{
private:
	typedef std::unique_ptr<EntityManager> EntityManagerHandle;
	typedef std::unique_ptr<SystemManager> SystemManagerHandle;

public:
	struct Instance
	{
		uuids::uuid Uuid;
		EntityManagerHandle EntityManager;
		SystemManagerHandle SystemManager;
	};

private:
	std::unordered_map<uuids::uuid, Instance> _Instances;

	EntityManagerHandle _EntityManager;
	SystemManagerHandle _SystemManager;

	ECSEngine();

public:

	static ECSEngine &Get()
	{
		static ECSEngine engine;
		return engine;
	}

	EntityManager *GetEntityManager()
	{
		return _EntityManager.get();
	}

	SystemManager *GetSystemManager()
	{
		return _SystemManager.get();
	}

	auto CreateInstance() -> Instance*
	{
		uuids::uuid instanceUuid(uuids::uuid_system_generator{}());

		_Instances[instanceUuid].Uuid = instanceUuid;

		return &_Instances[instanceUuid];
	}

	auto RemoveInstance(uuids::uuid uuid)
	{
		auto instance = _Instances.find(uuid);
		if (instance != _Instances.end()) {
			_Instances.erase(instance);
		}
	}

	void Update(float deltaTime);
};

}
