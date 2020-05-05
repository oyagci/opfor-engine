#include "Engine.hpp"
#include "WorldObject.hpp"
#include "MeshRendererSystem.hpp"

using namespace engine;

int main()
{
	Engine::Instance().CreateComponentSystem<MeshRendererSystem>();
	auto worldObject = Engine::Instance().CreateEngineObject<WorldObject>();
	(void)worldObject;
	return engine::Engine::Instance().Run();
}
