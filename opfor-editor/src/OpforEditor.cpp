#include "opfor.hpp"

#include "opfor/layers/ImGuiLayer.hpp"

#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/SkyboxComponent.hpp"

class OpforEditor : public opfor::Application
{
public:
	OpforEditor() : opfor::Application()
	{
		auto player = CreateEntity<PlayerCameraComponent, TransformComponent>();

		PlayerCameraComponent p;
			p.projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 1000000.0f);
			p.model = glm::mat4(1.0f);
			p.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			p.viewProjection = p.projection * p.view;
			p.exposure = 2.0f;
		player->Set(p);
		player->SetName("Player");

		TransformComponent t;
			t.direction = glm::vec3(0.0f, 0.0f, 1.0f);
			t.position = glm::vec3(0.0f, 18.0f, -0.5f);
		player->Set(t);

		auto skybox = CreateEntity<SkyboxComponent>();

		opfor::Application::Get().LoadLevel("assets/levels/pbr.level");
	}
};

namespace opfor {

UniquePtr<Application> CreateApplication()
{
	return MakeUnique<OpforEditor>();
}

}
