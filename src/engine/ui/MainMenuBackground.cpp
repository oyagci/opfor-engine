#include "MainMenuBackground.hpp"
#include "UIScene.hpp"

MainMenuBackground::MainMenuBackground(IUIScene *parent) : ASceneComponent(parent) {
	TextureManager::instance().createTexture("MenuBackground", "img/bg.png", {
		{ GL_TEXTURE_WRAP_S, GL_REPEAT },
		{ GL_TEXTURE_WRAP_T, GL_REPEAT },
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
	});

	_size = parent->getSize();

	glm::vec3 bg[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(_size.x, 0.0f, 0.0f),
		glm::vec3(_size.x, _size.y, 0.0f),
		glm::vec3(0.0f, _size.y, 0.0f),
	};
	glm::uvec3 inds[] = {
		glm::uvec3(0, 1, 2),
		glm::uvec3(0, 2, 3),
	};
	glm::vec2 tex[] = {
		glm::vec2(0.0f,            0.0f),
		glm::vec2(_size.x / 64.0f, 0.0f),
		glm::vec2(_size.x / 64.0f, _size.y / 64.0f),
		glm::vec2(0.0f,            _size.y / 64.0f),
	};

	for (auto const &p : bg) {
		_background.addPosition(p);
	}
	for (auto const &i : inds) {
		_background.addTriangle(i);
	}
	for (auto const &t : tex) {
		_background.addUv(t);
	}
	_background.build();
}
