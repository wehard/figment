#pragma once

#include "Figment.h"
#include "AStar.h"
#include "Graph.h"

using namespace Figment;

class Shapes : public Layer
{
public:
    bool RunningAStar = false;
public:
    explicit Shapes(WebGPUContext &context);
    ~Shapes() override = default;
    void OnAttach() override;
    void OnDetach() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;

    void RunAStar();
private:
    void CreateGraph(int width, int height);
private:
    struct Point
    {
        glm::vec3 Position;
        glm::vec4 Color;
        std::vector<Point *> Neighbors;
        bool Disabled = false;
    };

    float m_RunAStarInterval = 1.5f;
    float m_TimeSinceLastRun = 0.0f;

    ShapeRenderer m_ShapeRenderer;
    WebGPUContext &m_Context;
    OrthographicCamera m_Camera;

    std::unique_ptr<AStar<Point>::SearchResult> m_AStarResult = nullptr;

    Graph<Point> m_Graph;
};
