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

	EntityManagerHandle _EntityManager;
	SystemManagerHandle _SystemManager;

public:
	ECSEngine();

	EntityManager *GetEntityManager()
	{
		return _EntityManager.get();
	}

	SystemManager *GetSystemManager()
	{
		return _SystemManager.get();
	}

	void Update(float deltaTime);
};

}
