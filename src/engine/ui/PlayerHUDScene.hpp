#pragma once

#include "UIScene.hpp"
#include "Image.hpp"

class PlayerHUDScene : public IUIScene
{
public:
	PlayerHUDScene(UI *uiController, glm::vec2 size) : IUIScene(uiController, size)
	{
		_crosshairImage = createSceneComponent<Image>();
		_crosshairImage->setImage(18.0f, 18.0f, "img/crosshair.png");
		_crosshairImage->setAnchor(Anchor::Center);
		_crosshairImage->setOrigin(Origin::Center);
	}
private:
	std::shared_ptr<Image> _crosshairImage;
};
