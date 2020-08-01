#include "SceneComponent.hpp"
#include "UI.hpp"
#include "UIScene.hpp"

void ASceneComponent::call(std::string const &funcName)
{
	_scene->call(funcName);
}

void ASceneComponent::setSize(glm::vec2 size)
{
	_size = size;
}

glm::vec2 ASceneComponent::getScreenPosition() const
{
	glm::vec2 position(0.0f);
	glm::vec2 anchorOff = calculateOffset(getAnchor(), getSize());
	glm::mat4 modelMatrix(1.0f);

	glm::vec2 const &sceneSize = _scene->getSize();

	switch (getOrigin()) {
	case Origin::TopLeft:
		position = glm::vec2(0.0f, sceneSize.y);
		break ;
	case Origin::Top:
		position = glm::vec2(sceneSize.x / 2.0f, sceneSize.y);
		break ;
	case Origin::TopRight:
		position = glm::vec2(sceneSize.x, sceneSize.y);
		break ;
	case Origin::Left:
		position = glm::vec2(0.0f, sceneSize.y / 2.0f);
		break ;
	case Origin::Right:
		position = glm::vec2(sceneSize.x, sceneSize.y / 2.0f);
		break ;
	case Origin::Center:
		position = sceneSize / 2.0f;
		break ;
	case Origin::BottomLeft:
		position = glm::vec2(0.0f, 0.0f);
		break ;
	case Origin::Bottom:
		position = glm::vec2(sceneSize.x / 2.0f, 0.0f);
		break ;
	case Origin::BottomRight:
		position = glm::vec2(sceneSize.x, 0.0f);
		break ;
	};

	auto offset = getOffset();
	position += anchorOff;
	position += offset;

	modelMatrix = glm::translate(modelMatrix, glm::vec3(position, 0.0f));
	glm::vec3 screenPosition = glm::vec4(position, 0.0f, 1.0f) * modelMatrix;

	return glm::vec2(screenPosition.x, screenPosition.y);
}
