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
    constexpr static uint32_t s_AirColor = 0xffffb273;
    constexpr static uint32_t s_SandColor = 0xff0000ff;
    constexpr static uint32_t s_WaterColor = 0xffff0000;

    struct Coord
    {
        int X;
        int Y;
    };

    struct Particle
    {
        Coord Position;
        Coord PreviousPosition;
        uint32_t Color;
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
    struct TextureData
    {
        uint32_t Width;
        uint32_t Height;
    };

    WebGPUContext &m_Context;
    PerspectiveCamera &m_Camera;
    PixelCanvas *m_PixelCanvas;
    WebGPUShader *m_ComputeShader;
    WebGPUUniformBuffer<TextureData> *m_UniformBuffer;
    WebGPUBuffer<Particle> *m_ParticleBuffer;

    bool CanMove(PixelCanvas &canvas, int x, int y);
    void UpdateSandParticle(int x, int y);
    void UpdateWaterParticle(int x, int y);
    bool m_Dirty = false;

    constexpr static uint32_t s_Width = 320;
    constexpr static uint32_t s_Height = 200;
};
