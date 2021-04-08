#pragma once

#include <Refureku/Refureku.h>
#include <generated/EditorMaxProperty.rfk.h>

namespace opfor OP4NAMESPACE()
{

/**
 * @brief Property to describe how high the value can be set inside the editor
 */
class OP4CLASS() Max : public rfk::Property
{
  public:
    Max(float value) : _max(value)
    {
    }

    auto Get() const
    {
        return _max;
    }

  private:
    const float _max;

    Max_GENERATED
};

} // namespace opfor

File_GENERATED
