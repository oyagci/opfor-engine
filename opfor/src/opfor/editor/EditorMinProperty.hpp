#pragma once

#include <Refureku/Refureku.h>
#include <generated/EditorMinProperty.rfk.h>

/**
 * @brief Property to describe how low the value can be set inside the editor
 */
class OP4CLASS() Min : public rfk::Property
{
public:
    Min(float value) : _min(value)
    {
    }

    auto Get() const
    {
        return _min;
    }

  private:
    const float _min;

    Min_GENERATED
};

File_GENERATED
