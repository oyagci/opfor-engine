#pragma once

#include "ecs/System.hpp"
#include "engine/renderer/Renderer.hpp"

class EndSceneSystem : public ecs::ComponentSystem
{
public:
	void OnUpdate(float) override
	{
		opfor::Renderer::EndScene();
	}
};
