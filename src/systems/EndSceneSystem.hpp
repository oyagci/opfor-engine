#pragma once

#include "ecs/System.hpp"
#include "engine/renderer/Renderer.hpp"
#include "engine/Engine.hpp"

class EndSceneSystem : public ecs::ComponentSystem
{
public:
	void OnUpdate(float) override
	{
		opfor::Renderer::CopyDefaultFramebufferTo(opfor::Engine::Get().GetViewport(), opfor::CopyTarget::ColorBufferBit);
		opfor::Renderer::EndScene();
	}
};
