#include "Engine.hpp"
#include "WorldObject.hpp"
#include "MeshRendererSystem.hpp"
#include "DummyComponent.hpp"

using namespace engine;

int main()
{
	auto &engine = Engine::Instance();
	engine.CreateComponentSystem<MeshRendererSystem>();

	auto worldObject = engine.CreateEngineObject<WorldObject>();
	(void)worldObject;

	auto worldEntity = engine.CreateEntity<MeshComponent, DummyComponent>();

	return engine::Engine::Instance().Run();
}
