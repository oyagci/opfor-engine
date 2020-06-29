#pragma once

#include "ecs/System.hpp"
#include "engine/renderer/Renderer.hpp"
#include "Engine.hpp"

class BeginSceneSystem : public ecs::ComponentSystem
{
public:
	void OnUpdate(float) override
	{
		opfor::Renderer::BeginScene();
	}
};
