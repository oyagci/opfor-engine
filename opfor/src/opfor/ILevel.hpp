#pragma once

#include <string>

namespace ecs {
	class IEntityBase;
}

namespace opfor {

class ILevel
{
public:
	virtual ~ILevel() = default;

	virtual Vector<ecs::IEntityBase*> GetEntities() const = 0;
};

}
