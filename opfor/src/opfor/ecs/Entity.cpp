#include "Entity.hpp"
#include "components/TransformComponent.hpp"

namespace ecs {

unsigned int IEntityBase::NextEntityID = 0;

template <>
void IEntityBase::RegisterComponents<TransformComponent>()
{
	Components[GetTypeIndex<TransformComponent>()] = std::make_unique<TransformComponent>(*this);
}

template <>
void IEntityBase::Set<TransformComponent>(TransformComponent const &data) = delete;

}
