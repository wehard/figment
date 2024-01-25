#include "LayerStack.h"

namespace Figment
{
    LayerStack::~LayerStack()
    {
        for (Layer *layer : m_Layers)
        {
            delete layer;
        }
    }

    void LayerStack::AddLayer(Layer *layer)
    {
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
    }

    void LayerStack::AddOverlay(Layer *overlay)
    {
        m_Layers.emplace_back(overlay);
    }

    void LayerStack::RemoveLayer(Layer *layer)
    {
        m_Layers.erase(std::find(m_Layers.begin(), m_Layers.end(), layer));
    }
}
