#include <fmt/format.h>
#include <utility>
#include <string>

#include "Engine.hpp"
#include "lazy.hpp"
#include "Logger.hpp"

#include "systems/BeginSceneSystem.hpp"
#include "systems/EndSceneSystem.hpp"
#include "systems/CameraMovementSystem.hpp"
#include "systems/SkyboxRendererSystem.hpp"
#include "systems/MeshRendererSystem.hpp"
#include "systems/ImguiSystem.hpp"
#include "systems/LuaSystem.hpp"

using namespace opfor;

int main()
{
	Logger::Verbose("Hello World\n");
	fmt::print("{} - {}\n", LUA_COPYRIGHT, LUA_AUTHORS);

	auto &engine = Engine::Get();
	engine.CreateComponentSystem<CameraMovementSystem>();
	engine.CreateComponentSystem<BeginSceneSystem>();
	engine.CreateComponentSystem<MeshRendererSystem>();
	engine.CreateComponentSystem<SkyboxRendererSystem>();
	engine.CreateComponentSystem<EndSceneSystem>();
	engine.CreateComponentSystem<ImguiSystem>();
	// engine.CreateComponentSystem<LuaSystem>();

	opfor::TextureParameterList texParams = {
		{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::WrapT,         opfor::TextureParameterValue::Repeat },
		{ opfor::TextureParameterType::WrapS,         opfor::TextureParameterValue::Repeat },
	};

	auto prototype_tile = TextureManager::Get().Create2D("prototype_tile_8");
	auto img = opfor::ImageLoader::Load("./img/prototype_tile_8.png");

	prototype_tile->SetDataType(opfor::DataType::UnsignedByte);
	prototype_tile->SetHasAlpha(img.nchannel == 4);
	prototype_tile->SetIsSRGB(true);
	prototype_tile->SetInputFormat(img.nchannel == 4 ? opfor::DataFormat::RGBA : opfor::DataFormat::RGB);
	prototype_tile->SetOutputFormat(img.nchannel == 4 ? opfor::DataFormat::RGBA : opfor::DataFormat::RGB);
	prototype_tile->SetSize(img.width, img.height);
	prototype_tile->SetTextureParameters(texParams);
	prototype_tile->SetData(img.data.get());
	prototype_tile->Build();

	auto default_normal = TextureManager::Get().Create2D("default_normal");
	img = opfor::ImageLoader::Load("./img/default_normal.png");

	default_normal->SetDataType(opfor::DataType::UnsignedByte);
	default_normal->SetHasAlpha(img.nchannel == 4);
	default_normal->SetIsSRGB(true);
	default_normal->SetInputFormat(img.nchannel == 4 ? opfor::DataFormat::RGBA : opfor::DataFormat::RGB);
	default_normal->SetOutputFormat(img.nchannel == 4 ? opfor::DataFormat::RGBA : opfor::DataFormat::RGB);
	default_normal->SetSize(img.width, img.height);
	default_normal->SetTextureParameters(texParams);
	default_normal->SetData(img.data.get());
	default_normal->Build();

	auto player = engine.CreateEntity<PlayerCameraComponent, TransformComponent>();

	PlayerCameraComponent p;
		p.projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 10000.0f);
		p.model = glm::mat4(1.0f);
		p.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		p.viewProjection = p.projection * p.view;
		p.exposure = 2.0f;
	player->Set(p);
	player->SetName("Player");

	TransformComponent t;
		t.direction = glm::vec3(0.0f, 0.0f, 1.0f);
		t.position = glm::vec3(0.0f, 18.0f, -0.5f);
	player->Set(t);

	auto __attribute__((unused)) skybox = engine.CreateEntity<SkyboxComponent>();

	auto display = engine.CreateEntity<DisplayComponent>();
	DisplayComponent d;
	d.window = engine.GetWindow();
	display->Set(d);
	display->SetName("Display");

	return opfor::Engine::Get().Run();
}
