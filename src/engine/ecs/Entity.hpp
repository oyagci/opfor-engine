#pragma once

#include <type_traits>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "Component.hpp"
#include <iostream>

namespace ecs {

typedef std::type_index TypeIndex;

template <typename T>
TypeIndex const GetTypeIndex()
{
	return std::type_index(typeid(T));
}

class IEntityBase
{
protected:
	template <typename U>
	static bool constexpr is_component_base()
	{
		return std::is_base_of<IComponentBase, U>::value;
	}

	template <typename U, typename V, typename ... UTypes>
	static bool constexpr is_component_base()
	{
		return std::is_base_of<IComponentBase, U>::value && is_component_base<V, UTypes...>();
	}

	template <typename U>
	void RegisterComponents()
	{
		Components[GetTypeIndex<U>()] = std::make_unique<U>();
	}

	template <typename U, typename V, typename ... UTypes>
	void RegisterComponents()
	{
		RegisterComponents<U>();
		RegisterComponents<V, UTypes...>();
	}

	template <typename U>
	U &GetComponent()
	{
		static_assert(std::is_base_of<IComponentBase, U>::value, "typename U must de derived from IComponentBase");

		if (!HasComponents<U>()) { throw MissingComponentException(); }

		return *dynamic_cast<U*>(Components[GetTypeIndex<U>()].get());
	}

protected:
	std::unordered_map<TypeIndex, std::unique_ptr<IComponentBase>> Components;

public:
	virtual ~IEntityBase() = default;

	template <typename U>
	bool HasComponents() const
	{
		static_assert(std::is_base_of<IComponentBase, U>::value, "typename U must de derived from IComponentBase");

		return Components.find(GetTypeIndex<U>()) != Components.end();
	}

	template <typename U, typename V, typename ... UTypes>
	bool HasComponents() const
	{
		return Components.find(GetTypeIndex<U>()) != Components.end() &&
			HasComponents<V, UTypes...>();
	}

	template <typename U>
	void SetComponentData(U &data)
	{
		static_assert(std::is_base_of<IComponentBase, U>::value, "typename U must de derived from IComponentBase");
		GetComponent<U>() = data;
	}

	template <typename U>
	U const &GetComponentData() const
	{
		static_assert(std::is_base_of<IComponentBase, U>::value, "typename U must de derived from IComponentBase");

		if (!HasComponents<U>()) { throw MissingComponentException(); }

		auto component = Components.find(GetTypeIndex<U>());

		return *dynamic_cast<U*>(component->second.get());
	}

	class MissingComponentException : public std::exception
	{
		public:
			const char *what() const noexcept
			{
				return "Component does not exists";
			}
	};
};

template <typename T, typename ... Types>
class IEntity : public IEntityBase
{
public:
	IEntity()
	{
		RegisterComponents<T, Types...>();
	}

	static_assert(is_component_base<T, Types...>(),
		"InputTypes parameter pack must be all derived from IComponent");
};

}
