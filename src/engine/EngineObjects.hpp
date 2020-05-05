#pragma once

#include <vector>
#include <memory>
#include "ecs/ecs.hpp"

namespace ecs { class ECSEngine; }

namespace engine
{

class EngineObject
{
	friend class Engine;

protected:
	ecs::ECSEngine *_ecs;
private:
	std::vector<EngineObject*> _subobjects;

public:
	EngineObject(ecs::ECSEngine *engine) : _ecs(engine) {}
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

	template <typename T, typename ... ArgTypes>
	ecs::IEntity<T, ArgTypes...> *CreateEntity()
	{
		return _ecs->GetEntityManager()->CreateEntity<T, ArgTypes...>();
	}

	template <typename ... ArgTypes>
	std::vector<ecs::IEntity<ArgTypes...>*> GetEntities()
	{
		return _ecs->GetEntityManager()->GetEntities<ArgTypes...>();
	}
};

}
