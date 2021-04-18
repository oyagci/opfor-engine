#include "AnimationSystem.hpp"

#include <opfor/core/types/Vec2.hpp>
#include <src/components/MeshComponent.hpp>
#include <src/components/TransformComponent.hpp>

namespace opfor
{

// Basic linear interpolation.
constexpr float interpolate(Vec2 p1, Vec2 p2, unsigned x)
{
    return p1.y + (x - p1.x) / (p2.x - p1.x) * (p2.y - p1.y);
}

static Vec3 interpolate(Vec3 v)
{
  return Vec3{ };
}

void AnimationSystem::OnUpdate(float dt)
{

    static constexpr auto ANIMATION_FRAMERATE = 1. / 24.; // 24 FPS
    auto entities = GetEntities<AnimationComponent, MeshComponent, TransformComponent>();

    for (auto &entity : entities)
    {
        auto &animation = entity->Get<AnimationComponent>();
        const auto apply_to_channels = [&animation](auto action) {
            action(animation.translate);
            action(animation.rotate);
            action(animation.scale);
        };

        if (dt - animation.last_update < ANIMATION_FRAMERATE)
        {
            continue;
        }
        animation.last_update = dt;

        if (animation.current_frame >= animation.length)
        {
            apply_to_channels([](auto &channel) { channel.current_keyframe = std::nullopt; });
            animation.current_frame = 0;
        }

        apply_to_channels([&animation](auto &channel) {
            const auto &current_kf_idx = channel.current_keyframe;

            if (current_kf_idx.has_value())
            {
              const auto& current_kf = channel.keyframes[current_kf_idx.value()];
              
              if constexpr (std::is_same<decltype(current_kf.value), Vec3>::value) {
                
              }
            }

            const unsigned next_kf_idx = current_kf_idx.has_value() ? current_kf_idx.value() + 1 : 0u;
            if (next_kf_idx >= channel.keyframes.size())
                return;

            const auto &next_kf = channel.keyframes[next_kf_idx];

            if (next_kf.target_frame == animation.current_frame)
                channel.current_keyframe = next_kf_idx;
        });

        auto &current_keyframe = animation.keyframes[animation.current_keyframe];

        if (animation.current_keyframe != 0)
        {
            using Keyframe = AnimationComponent::Keyframe;

            const auto &last_keyframe = animation.keyframes[animation.current_keyframe - 1];

            // TODO Handle more interpolation types
            OP4_CORE_ASSERT(current_keyframe.interpolation == Keyframe::Interpolation::Linear,
                            "Unsupported interpolation type");


            auto &transform = entity->Get<TransformComponent>();

            {
                Vec3 translate = {
                    interpolate({last_keyframe.frame_target, last_keyframe.translate.x},
                                {current_keyframe.frame_target, current_keyframe.translate.x}, animation.current_frame),
                    interpolate({last_keyframe.frame_target, last_keyframe.translate.y},
                                {current_keyframe.frame_target, current_keyframe.translate.y}, animation.current_frame),
                    interpolate({last_keyframe.frame_target, last_keyframe.translate.z},
                                {current_keyframe.frame_target, current_keyframe.translate.z},
                                animation.current_frame)};

                transform.position = transform.position + translate;
            }

            {
                Quat rotate = {
                    interpolate({last_keyframe.frame_target, last_keyframe.rotate.x},
                                {current_keyframe.frame_target, current_keyframe.rotate.x}, animation.current_frame),
                    interpolate({last_keyframe.frame_target, last_keyframe.rotate.y},
                                {current_keyframe.frame_target, current_keyframe.rotate.y}, animation.current_frame),
                    interpolate({last_keyframe.frame_target, last_keyframe.rotate.z},
                                {current_keyframe.frame_target, current_keyframe.rotate.z},
                                animation.current_frame),
                    interpolate({last_keyframe.frame_target, last_keyframe.rotate.w},
                                {current_keyframe.frame_target, current_keyframe.rotate.w},
                                animation.current_frame)};


                transform.rotation = transform.rotation + rotate;
            }

            {
                Vec3 scale = {
                    interpolate({last_keyframe.frame_target, last_keyframe.scale.x},
                                {current_keyframe.frame_target, current_keyframe.scale.x}, animation.current_frame),
                    interpolate({last_keyframe.frame_target, last_keyframe.scale.y},
                                {current_keyframe.frame_target, current_keyframe.scale.y}, animation.current_frame),
                    interpolate({last_keyframe.frame_target, last_keyframe.scale.z},
                                {current_keyframe.frame_target, current_keyframe.scale.z},
                                animation.current_frame)};

                transform.scale = transform.scale + scale;
            }
        }
        if (++animation.current_frame == current_keyframe.frame_target)
        {
            ++animation.current_keyframe;
        }
    }
}

void AnimationSystem::AddAnimation(AnimationComponent animation)
{
    auto *animation_entity = EntityMgr->CreateEntity<AnimationComponent>();
    animation_entity->Set<AnimationComponent>(std::move(animation));
}
} // namespace opfor
