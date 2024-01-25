#pragma once

#include "Layer.h"
#include <vector>

namespace Figment
{
    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        void AddLayer(Layer *layer);
        void AddOverlay(Layer *overlay);
        void RemoveLayer(Layer *layer);

        std::vector<Layer *> &GetLayers()
        { return m_Layers; }

        std::vector<Layer *>::iterator begin()
        { return m_Layers.begin(); }
        std::vector<Layer *>::iterator end()
        { return m_Layers.end(); }
        std::vector<Layer *>::reverse_iterator rbegin()
        { return m_Layers.rbegin(); }
        std::vector<Layer *>::reverse_iterator rend()
        { return m_Layers.rend(); }

        std::vector<Layer *>::const_iterator begin() const
        { return m_Layers.begin(); }
        std::vector<Layer *>::const_iterator end() const
        { return m_Layers.end(); }
        std::vector<Layer *>::const_reverse_iterator rbegin() const
        { return m_Layers.rbegin(); }
        std::vector<Layer *>::const_reverse_iterator rend() const
        { return m_Layers.rend(); }

    private:
        std::vector<Layer *> m_Layers;
        long m_LayerInsertIndex = 0;
    };
}
