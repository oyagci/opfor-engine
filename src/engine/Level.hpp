#pragma once

namespace engine {

class ILevel
{
public:
	virtual ~ILevel() {}

	virtual void Load() = 0;
	virtual void Unload() = 0;
};

}
