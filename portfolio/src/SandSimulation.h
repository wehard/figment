#pragma once

#include "Figment.h"

using namespace Figment;

template<typename K, typename V>
class bimap
{
public:
    bimap() = default;
    bimap(std::initializer_list<std::pair<K, V>> init)
    {
        for (auto &pair : init)
        {
            insert(pair.first, pair.second);
        }
    }

    V operator[](K k)
    {
        return m_KVMap[k];
    }

    K operator[](V v)
    {
        return m_VKMap[v];
    }

private:
    std::map<K, V> m_KVMap;
    std::map<V, K> m_VKMap;

    void insert(K k, V v)
    {
        m_KVMap[k] = v;
        m_VKMap[v] = k;
    }
};

class SandSimulation : public Figment::Layer
{
public:
    constexpr static uint32_t s_NoneColor = 0xff000000;
    constexpr static uint32_t s_AirColor = 0xffa89ba1;
    constexpr static uint32_t s_SandColor = 0xff004b96;
    constexpr static uint32_t s_WaterColor = 0xffff0000;

    enum class ElementType
    {
        None,
        Air,
        Sand,
        Water
    };

    bimap<ElementType, uint32_t> m_ElementColorMap = {
            { ElementType::None, s_NoneColor },
            { ElementType::Air, s_AirColor },
            { ElementType::Sand, s_SandColor },
            { ElementType::Water, s_WaterColor },
    };

    enum class Direction
    {
        UpLeft,
        Up,
        UpRight,
        Left,
        None,
        Right,
        DownLeft,
        Down,
        DownRight,
    };

    struct Element
    {
        uint32_t Color = 0xff000000;
        ElementType Type = ElementType::None;
        virtual bool CanDisplace(ElementType elementType)
        { return false; };
    };

    struct Sand : public Element
    {
        Sand() : Element()
        {
            Color = s_SandColor;
            Type = ElementType::Sand;
        }

        bool CanDisplace(ElementType elementType) override
        {
            switch (elementType)
            {
            case ElementType::Air:
            case ElementType::Water:
                return true;
            case ElementType::None:
            case ElementType::Sand:
                return false;
            }
            return false;
        }

        std::vector<Direction> m_MoveDirections = {
                Direction::Down,
                Direction::DownLeft,
                Direction::DownRight
        };

        Direction TryGetDisplaceDirection(std::array<ElementType, 9> neighbours)
        {
            for (auto dir : m_MoveDirections)
            {
                if (CanDisplace(neighbours[(int)dir]))
                    return dir;
            }
            return Direction::None;
        }
    };

    struct Water : public Element
    {
        Water() : Element()
        {
            Color = s_WaterColor;
            Type = ElementType::Water;
        }

        bool CanDisplace(ElementType elementType) override
        {
            switch (elementType)
            {
            case ElementType::None:
            case ElementType::Air:
                return true;
            case ElementType::Sand:
            case ElementType::Water:
                return false;
            }
            return false;
        }
    };

public:
    SandSimulation(WebGPUContext &context, PerspectiveCamera &camera);
    ~SandSimulation() override = default;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(Figment::AppEvent event, void *eventData) override;
private:
    WebGPUContext &m_Context;
    PerspectiveCamera &m_Camera;
    PixelCanvas *m_PixelCanvas;

    std::array<ElementType, 9> GetNeighbours(int x, int y);
    bool CanMove(PixelCanvas &canvas, int x, int y);
    void UpdateSand(int x, int y);
    void UpdateWater(int x, int y);
    bool m_Dirty = false;

    constexpr static uint32_t s_Width = 320;
    constexpr static uint32_t s_Height = 200;
};
