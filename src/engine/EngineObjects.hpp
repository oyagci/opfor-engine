#pragma once

#include <vector>
#include <memory>
#include "engine/ecs/ecs.hpp"

namespace engine
{

class EngineObject
{
private:
	std::vector<EngineObject*> _subobjects;

public:
	virtual ~EngineObject() {}

	virtual void Update() {}
	virtual void FixedUpdate() {}

	std::vector<EngineObject*> GetSubobjects()
	{
		return _subobjects;
	}

	template <typename T, typename ... ArgTypes>
	T *CreateSubobject(ArgTypes... args) {

		static_assert(std::is_base_of<EngineObject, T>::value);

		auto obj = std::make_unique<T>(std::forward<ArgTypes>(args)...);
		auto ret = obj.get();

		_subobjects.push_back(std::move(obj));

		return ret;
	}
};

}
