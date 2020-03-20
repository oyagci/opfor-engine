#include "EngineObjects.hpp"
#include <iostream>
#include "Engine.hpp"

class WorldObject : public engine::EngineObject
{
public:
	WorldObject()
	{
	}

	void Update() override
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}
};
