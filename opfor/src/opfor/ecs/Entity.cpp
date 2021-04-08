#include "Entity.hpp"
#include "components/TransformComponent.hpp"

namespace opfor
{

namespace ecs
{

unsigned int IEntityBase::NextEntityID = 0;

template <> void IEntityBase::Set<TransformComponent>(TransformComponent const &data) = delete;

} // namespace ecs

} // namespace opfor
