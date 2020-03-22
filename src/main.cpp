#include "Engine.hpp"
#include "WorldObject.hpp"

using namespace engine;

int main()
{
	auto worldObject = Engine::Instance().CreateEngineObject<WorldObject>();
	(void)worldObject;
	return engine::Engine::Instance().Run();
}
