#pragma once

#include <Refureku/Refureku.h>
#include <generated/EditorReadOnlyProperty.rfk.h>

/**
 * @brief Property to prevent a value from being edited from the editor
 */
class OP4CLASS() ReadOnly : public rfk::Property
{
public:
    ReadOnly() = default;

    ReadOnly_GENERATED
};

File_GENERATED
