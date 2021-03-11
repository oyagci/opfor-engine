#pragma once

#include "Layer.hpp"

namespace opfor
{

class LayerStack
{
  private:
    using Container = std::vector<Layer *>;

    Container _Layers;
    size_t _LastInsertIndex = 0;

  public:
    LayerStack();
    ~LayerStack();

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *overlay);
    void PopLayer(Layer *layer);
    void PopOverlay(Layer *overlay);

    // Iterators
    // =========
    Container::iterator begin()
    {
        return _Layers.begin();
    }
    Container::iterator end()
    {
        return _Layers.end();
    }
    Container::const_iterator begin() const
    {
        return _Layers.begin();
    }
    Container::const_iterator end() const
    {
        return _Layers.end();
    }

    // Reverse-Iterators
    // =================
    Container::reverse_iterator rbegin()
    {
        return _Layers.rbegin();
    }
    Container::reverse_iterator rend()
    {
        return _Layers.rend();
    }
    Container::const_reverse_iterator rbegin() const
    {
        return _Layers.rbegin();
    }
    Container::const_reverse_iterator rend() const
    {
        return _Layers.rend();
    }
};

} // namespace opfor
