#include "UIScene.hpp"
#include "UI.hpp"

void IUIScene::call(std::string const &name)
{
	_uiController->call(name);
}
