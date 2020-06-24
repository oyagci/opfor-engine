#pragma once

#include <string>

namespace engine {

class ILevel
{
public:
	virtual ~ILevel() {}

	virtual void Load(const std::string &path) = 0;
	virtual void Unload() = 0;
};

}
