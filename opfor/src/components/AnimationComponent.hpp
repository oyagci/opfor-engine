#pragma once

#include <opfor/core/base.hpp>
#include <opfor/core/types/Vec3.hpp>
#include <opfor/core/types/Quat.hpp>
#include <opfor/ecs/Component.hpp>

namespace opfor
{
struct AnimationComponent : ecs::IComponentBase
{
    String name;

    float last_update;

    unsigned current_frame;
    unsigned length;

    template <typename T>
    struct Channel {
        enum class Interpolation
        {
            Linear
        } interpolation;

        struct Keyframe
        {
            unsigned target_frame;
            T value;
        };
        Vector<Keyframe> keyframes;

        Optional<unsigned> current_keyframe;
    };
    Channel<Vec3> translate;
    Channel<Quat> rotate;
    Channel<Vec3> scale;

};
} // namespace opfor
