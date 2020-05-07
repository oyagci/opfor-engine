#include "Engine.hpp"
#include "WorldObject.hpp"
#include "MeshRendererSystem.hpp"
#include "DummyComponent.hpp"
#include "engine/assimp/Model.hpp"

using namespace engine;

int main()
{
	auto &engine = Engine::Instance();
	engine.CreateComponentSystem<MeshRendererSystem>();

	auto worldObject = engine.CreateEngineObject<WorldObject>();
	(void)worldObject;

	auto backpack = assimp::Model("./backpack/backpack.obj");

	using BackpackMesh = ecs::IEntity<MeshComponent>;

	std::vector<BackpackMesh*> meshes;

	for (auto &m : backpack.getMeshes()) {
		BackpackMesh *b = engine.CreateEntity<MeshComponent>();

		MeshComponent comp;
		comp.mesh = std::move(m);
		b->SetComponentData<MeshComponent>(std::move(comp));

		meshes.push_back(b);
	}

	return engine::Engine::Instance().Run();
}
