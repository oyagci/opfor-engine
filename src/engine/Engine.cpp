#include "Engine.hpp"
#include "Time.hpp"
#include "utils/Settings.hpp"
#include "stb_image.h"

namespace engine
{

Engine::Engine()
{
	_display = std::make_unique<Display>("ft_vox", 1920, 1080);
	_display->enableCap(GL_DEPTH_TEST);
	_display->enableCap(GL_CULL_FACE);
	_display->enableCap(GL_BLEND);
	_display->setFullscreen(true);
	_display->showCursor(false);

	maths::transform t = { glm::vec3(32, 64, 32), glm::quat(), glm::vec3(1), nullptr };
	_camera = std::make_unique<Camera>(*_display, t);
	_camera->setProjection(glm::radians(80.0f), 0.1f, 1000.0f);

	_ui = std::make_unique<UI>(_display->getWidth(), _display->getHeight());

	_systemManager = _ecs.GetSystemManager();
	_entityManager = _ecs.GetEntityManager();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

int Engine::Run()
{
	Settings::instance().load("config.ini");

	while (!_display->isClosed())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Update();

		Time::instance().update();

		float deltaTime = Time::instance().getDeltaTime();

		_camera->update();
		_ecs.Update(deltaTime);

		_display->update();
		_display->updateInputs();

		_ui->update();
		_ui->render();
	}

	return 0;
}

void Engine::Update()
{
	UpdateObjects();
}

void Engine::UpdateObjects()
{
	for (auto &o : _engineObjects) {
		o->Update();
		UpdateSubobjects(o->GetSubobjects());
	}
}


void Engine::UpdateSubobjects(std::vector<EngineObject*> subobjects)
{
	for (auto &o : subobjects) {
		o->Update();
		UpdateSubobjects(o->GetSubobjects());
	}
}

}
