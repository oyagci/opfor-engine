#pragma once

#include "ecs/System.hpp"
#include "opfor/renderer/Renderer.hpp"
#include "Application.hpp"

class BeginSceneSystem : public ecs::ComponentSystem
{
public:
	void OnUpdate(float) override
	{
		opfor::Renderer::BeginScene();
	}
};
