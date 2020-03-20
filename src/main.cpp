#include "Engine.hpp"
#include "WorldObject.hpp"

int main()
{
	auto worldObject = engine::Engine::Instance().CreateEngineObject<WorldObject>();
	(void)worldObject;
	return engine::Engine::Instance().Run();
}
