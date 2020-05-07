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

	std::vector<BackpackMesh*> backpackMeshes;

	std::vector<lazy::graphics::Mesh> &meshes = backpack.getMeshes();
	std::vector<std::string> textures = backpack.getTextures();

	for (auto &m : meshes) {
		BackpackMesh *b = engine.CreateEntity<MeshComponent>();

		MeshComponent comp;
		comp.mesh = std::move(m);
		comp.textures = std::vector<std::string>(textures);
		b->SetComponentData<MeshComponent>(std::move(comp));

		backpackMeshes.push_back(b);
	}

	return engine::Engine::Instance().Run();
}
