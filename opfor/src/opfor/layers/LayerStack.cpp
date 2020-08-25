#include "LayerStack.hpp"

namespace opfor {

LayerStack::LayerStack() = default;

LayerStack::~LayerStack()
{
	for (auto layer : _Layers) {
		//layer->OnDetach();
	}
}

void LayerStack::PushLayer(Layer *layer)
{
	_Layers.emplace(_Layers.begin() + _LastInsertIndex, std::move(layer));
	_LastInsertIndex += 1;
}

void LayerStack::PushOverlay(Layer *layer)
{
	_Layers.emplace_back(std::move(layer));
}

void LayerStack::PopLayer(Layer *layer)
{
	auto it = std::find(_Layers.begin(), _Layers.end(), layer);

	if (it != _Layers.end()) {
		_Layers.erase(it);
		_LastInsertIndex -= 1;
	}
}

void LayerStack::PopOverlay(Layer *overlay)
{
	auto it = std::find(_Layers.begin(), _Layers.end(), overlay);

	if (it != _Layers.end()) {
		_Layers.erase(it);
	}
}

}
