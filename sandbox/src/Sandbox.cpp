#include "opfor.hpp"

#include "systems/CameraMovementSystem.hpp"

#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/SkyboxComponent.hpp"

class Sandbox : public opfor::Application
{
public:
	Sandbox() : opfor::Application()
	{
	}
};

opfor::UniquePtr<opfor::Application> opfor::CreateApplication()
{
	return MakeUnique<Sandbox>();
}
