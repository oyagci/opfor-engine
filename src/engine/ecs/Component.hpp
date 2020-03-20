#pragma once

namespace ecs {

struct IComponentBase
{
	virtual ~IComponentBase() {}
};

struct IComponent : IComponentBase
{
	virtual ~IComponent() {}
};

}
