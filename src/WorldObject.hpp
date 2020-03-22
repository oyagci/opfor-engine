#include "EngineObjects.hpp"
#include <iostream>
#include "Engine.hpp"
#include "MeshComponent.hpp"

class WorldObject : public engine::EngineObject
{
private:
	ecs::IEntity<MeshComponent> *TestMesh;

public:
	WorldObject()
	{
		TestMesh = CreateEntity<MeshComponent>();

		MeshComponent meshComponent;

		glm::vec3 positions[] = {
			{ -0.5f, -0.5f, 0.0f },
			{  0.5f, -0.5f, 0.0f },
			{  0.0f,  0.5f, 0.0f },
		};
		glm::uvec3 indices[] = {
			{ 0, 1, 2 }
		};
		for (auto const &p : positions) {
			meshComponent.mesh.addPosition(p);
		}
		for (auto const &i : indices) {
			meshComponent.mesh.addTriangle(i);
		}
		meshComponent.mesh.build();

		TestMesh->SetComponentData<MeshComponent>(meshComponent);
	}

	void Update() override
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}
};
