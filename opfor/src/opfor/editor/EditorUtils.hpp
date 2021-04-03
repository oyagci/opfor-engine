#pragma once
#include <opfor/core/types/String.hpp>
#include <opfor/core/types/Vec3.hpp>
#include <limits>

namespace opfor
{
class EditorUtils
{
  public:
    static bool DrawVec3Control(const String &label, Vec3 &values, float resetValue = 0.0f, float columnWidth = 100.0f);

    static bool DrawFloatControl(const String &label, float &value, float dragSpeed = 0.1f, float min = 0.0f,
                                 float max = 0.0f, float columnWidth = 100.0f);

    static bool DrawStringControl(const String &label, String &value, bool readOnly = false,
                                  float columnWidth = 100.0f);

    static bool DrawIntControl(const String &label, int &value, bool readOnly = false, int speed = 1, int min = 0,
                               int max = (std::numeric_limits<int>::max)(), float columnWidth = 100.0f);

    static bool DrawUIntControl(const String &label, int &value, bool readOnly = false, int speed = 1, int min = 0,
                                int max = (std::numeric_limits<int>::max)(), float columnWidth = 100.0f);
};

} // namespace opfor
