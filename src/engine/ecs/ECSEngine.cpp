#include "ECSEngine.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"
#include <memory>

namespace ecs {

ECSEngine::ECSEngine()
{
	_EntityManager = std::make_unique<EntityManager>();
	_SystemManager = std::make_unique<SystemManager>(_EntityManager.get());
}

void ECSEngine::Update(float deltaTime)
{
	_SystemManager->Update(deltaTime);
}

}
