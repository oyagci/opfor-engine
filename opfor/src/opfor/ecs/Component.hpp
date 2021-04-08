#pragma once

#include <Refureku/Refureku.h>
#include <generated/Component.rfk.h>

namespace opfor OP4NAMESPACE()
{

namespace ecs OP4NAMESPACE()
{

    struct OP4STRUCT() IComponentBase : rfk::Object
    {
        virtual ~IComponentBase()
        {
        }

        // Explicitly deleted by `rfk::Object`
        IComponentBase &operator=(IComponentBase const &)
        {
            return *this;
        }

        IComponentBase_GENERATED
    };

    struct IComponent : IComponentBase
    {
        virtual ~IComponent()
        {
        }
    };

} // namespace )

} // namespace opfor

File_GENERATED

