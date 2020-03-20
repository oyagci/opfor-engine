#include "Button.hpp"
#include "TextureManager.hpp"
#include "Anchor.hpp"
#include "TextComponent.hpp"

Button::Button(IUIScene *scene) : ASceneComponent(scene)
{
	setup(glm::vec2(640.0f, 64.0f), Anchor::Center);
}

void Button::setup(glm::vec2 size, Anchor anchorPoint)
{
	TextureManager::instance().createTexture("Button", "img/button.png", {
		{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST }
	});
	TextureManager::instance().createTexture("ButtonHovering", "img/button_hover.png", {
		{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST }
	});

	setAnchor(anchorPoint);
	setOrigin(Origin::Center);
	setSize(size);

	_onClick = [] { /* noop */ };

	_label = createSubComponent<Label>();
	_label->setOrigin(Origin::Center);
	_label->setAnchor(Anchor::Center);
	_label->setText("Hello");

	buildMesh();

	_canBeClicked = true;
	lazy::inputs::input::getMouse().attach(this);
}

void Button::buildMesh()
{
	glm::vec3 quad[] = {
		glm::vec3(0.0f,               0.0f,               0.0f),
		glm::vec3(0.0f + getSize().x, 0.0f,               0.0f),
		glm::vec3(0.0f + getSize().x, 0.0f + getSize().y, 0.0f),
		glm::vec3(0.0f,               0.0f + getSize().y, 0.0f),
	};
	glm::uvec3 inds[] = {
		glm::uvec3(0, 1, 2),
		glm::uvec3(0, 2, 3),
	};
	glm::vec2 tex[] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
	};

	_mesh = Mesh();
	for (auto const &p : quad) {
		_mesh.addPosition(p);
	}
	for (auto const &i : inds) {
		_mesh.addTriangle(i);
	}
	for (auto const &t : tex) {
		_mesh.addUv(t);
	}
	_mesh.build();
}

Button::~Button()
{
}

void Button::update()
{
}

void Button::draw(Shader &shader)
{
	if (_isHovering) {
		TextureManager::instance().bind("ButtonHovering", GL_TEXTURE0);
	}
	else {
		TextureManager::instance().bind("Button", GL_TEXTURE0);
	}
	_mesh.draw();
}

void Button::setText(std::string const &text)
{
	_label->setText(text);
}

void Button::onClickUpInside()
{
	_canBeClicked = false;
	_onClick();
}

void Button::onHover(bool val)
{
	_isHovering = val;
}

glm::vec4 Button::getObservedArea() const
{
	return glm::vec4(getScreenPosition(), getSize());
}
