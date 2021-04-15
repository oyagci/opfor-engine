#pragma once

#include <generated/Vec2.rfk.h>

namespace opfor OP4NAMESPACE()
{
    template <typename T>
    class AVec2
    {
      public:
        AVec2() = default;

        AVec2<T> operator*(float v)
        {
            return {x * v, y * v};
        }

        AVec2(T const x, T const y) : x(x), y(y)
        {
        }

        AVec2<T> operator-(AVec2<T> const &rhs) const
        {
            return {x - rhs.x, y - rhs.y};
        }

        T x = T(0);
        T y = T(0);
    };

    class OP4CLASS() Vec2 : public AVec2<float>
    {
      public:
        using AVec2<float>::AVec2;

        Vec2(AVec2<float> const &v) : AVec2<float>(v)
        {
        }

        Vec2_GENERATED
    };

    class OP4CLASS() UVec2 : public AVec2<unsigned int>
    {
      public:
        using AVec2<unsigned int>::AVec2;

        UVec2(AVec2<unsigned int> const &v) : AVec2<unsigned int>(v)
        {
        }

        UVec2_GENERATED
    };
} // namespace opfor

File_GENERATED
