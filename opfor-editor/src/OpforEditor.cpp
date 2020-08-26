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
		auto player = CreateEntity<TransformComponent>();
		auto sky = CreateEntity<SkyboxComponent>();
		opfor::Application::Get().LoadLevel("assets/levels/sky.level");
	}
};

namespace opfor {

UniquePtr<Application> CreateApplication()
{
	return MakeUnique<OpforEditor>();
}

}
