#include "Engine.hpp"
#include "engine/assimp/Model.hpp"
#include "systems/CameraMovementSystem.hpp"
#include "systems/MeshRendererSystem.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"

using namespace engine;

int main()
{
	auto &engine = Engine::Instance();
	engine.CreateComponentSystem<MeshRendererSystem>();
	engine.CreateComponentSystem<CameraMovementSystem>();

	auto player = engine.CreateEntity<PlayerCameraComponent, TransformComponent>();

	PlayerCameraComponent p;
		p.projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.01f, 10000.0f);
		p.model = glm::mat4(1.0f);
		p.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
		p.viewProjection = p.projection * p.view;
	player->SetComponentData<PlayerCameraComponent>(p);

	TransformComponent t;
		t.direction = glm::vec3(0.0f, 0.0f, 1.0f);
		t.position = glm::vec3(0.0f, 180.0f, -10.0f);
	player->SetComponentData<TransformComponent>(t);

	using MeshEntity = ecs::IEntity<MeshComponent>;

	auto sponza = assimp::Model("./Sponza/sponza.obj");
	std::vector<MeshEntity*> sponzaMeshes;
	{
		std::vector<assimp::Mesh> &meshes = sponza.getMeshes();

		for (auto &m : meshes) {
			MeshEntity *b = engine.CreateEntity<MeshComponent>();

			MeshComponent comp;
			comp.textures = std::vector<std::string>(m.getTextures());
			comp.mesh = std::move(m);
			b->SetComponentData<MeshComponent>(std::move(comp));

			sponzaMeshes.push_back(b);
		}
	}

	return engine::Engine::Instance().Run();
}
