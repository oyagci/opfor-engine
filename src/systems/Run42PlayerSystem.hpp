#pragma once

#include "ecs/System.hpp"
#include "components/TransformComponent.hpp"
#include "components/Run42Player.hpp"

class Run42PlayerSystem : public ecs::ComponentSystem
{
private:
public:
	void OnUpdate(float) override
	{
		auto players = GetEntities<Run42PlayerComponent, TransformComponent>();

		if (players.size() == 0) {
			return ;
		}

		TransformComponent &transform = players[0]->Get<TransformComponent>();
		auto &player = players[0]->Get<Run42PlayerComponent>();

		bool rgt = lazy::inputs::input::getKeyboard().getKeyDown(GLFW_KEY_D);
		bool lft = lazy::inputs::input::getKeyboard().getKeyDown(GLFW_KEY_A);

		if (rgt && (player.CurrentPosition == Run42PlayerComponent::Position::Left ||
					player.CurrentPosition == Run42PlayerComponent::Position::Middle)) {
			transform.position.z += 200.0f;

			if (player.CurrentPosition == Run42PlayerComponent::Position::Middle) {
				player.CurrentPosition = Run42PlayerComponent::Position::Right;
			}
			else {
				player.CurrentPosition = Run42PlayerComponent::Position::Middle;
			}
		}

		if (lft && (player.CurrentPosition == Run42PlayerComponent::Position::Right ||
					player.CurrentPosition == Run42PlayerComponent::Position::Middle)) {
			transform.position.z -= 200.0f;

			if (player.CurrentPosition == Run42PlayerComponent::Position::Middle) {
				player.CurrentPosition = Run42PlayerComponent::Position::Left;
			}
			else {
				player.CurrentPosition = Run42PlayerComponent::Position::Middle;
			}
		}
	}
};
