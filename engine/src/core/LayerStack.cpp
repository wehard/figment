#include "LayerStack.h"

namespace Figment
{
    LayerStack::~LayerStack()
    {
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
