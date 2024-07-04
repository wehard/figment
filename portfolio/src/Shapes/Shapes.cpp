#include "Shapes.h"
#include "AStar.h"

Shapes::Shapes(WebGPUContext &context) : Layer("Shapes", true), m_Context(context),
        m_ShapeRenderer(context)
{
    m_Camera = OrthographicCamera((float)context.GetSwapChainWidth(), (float)context.GetSwapChainHeight());
    m_Camera.Zoom(50.0f, { context.GetSwapChainWidth() / 2, context.GetSwapChainHeight() / 2 });
    m_Camera.SetPosition({ 4.5, 4.5, 0 });

    const int width = 10;
    const int height = 10;
    m_Points.resize(width * height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float rx = Random::Float();
            float ry = Random::Float();
            float scale = 0.0;
            m_Points[x + y * width] =
                    Point {
                            .Position = { (float)x + rx * scale, (float)y + ry * scale, 0 },
                            .Color = { 0.8f, 0.2f, 0.3f, 1.0f }
                    };
        }
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (x < 9)
            {
                m_Points[x + y * 10].Neighbors.push_back(&m_Points[x + 1 + y * 10]);
            }
            if (y < 9)
            {
                m_Points[x + y * 10].Neighbors.push_back(&m_Points[x + (y + 1) * 10]);
            }
            if (x < 9 && y < 9)
            {
                m_Points[x + y * 10].Neighbors.push_back(&m_Points[x + 1 + (y + 1) * 10]);
            }
        }
    }
}

void Shapes::OnAttach()
{

}

void Shapes::OnDetach()
{

}

void Shapes::OnEnable()
{
    // m_Camera.SetPosition(glm::vec3(0.0, 0.0, 20.0));
}

void Shapes::OnDisable()
{

}

void Shapes::OnUpdate(float deltaTime)
{
    static glm::vec3 offset = { -4.5f, -4.5f, 0.0f };
    m_ShapeRenderer.Begin(m_Camera);
    for (auto &point : m_Points)
    {
        m_ShapeRenderer.SubmitCircle(point.Position, point.Color, 0.5, -1);
        for (auto &neighbor : point.Neighbors)
        {
            // m_ShapeRenderer.SubmitLine(point.Position, neighbor->Position, { 1, 1, 1, 1 }, -1);
        }
    }
    if (m_AStarResult)
    {
        for (size_t i = 0; i < m_AStarResult->Path.size() - 1; i++)
        {
            m_ShapeRenderer.SubmitLine(m_AStarResult->Path[i]->UserData.Position,
                    m_AStarResult->Path[i + 1]->UserData.Position, { 0, 1, 0, 1 }, -1);
        }
    }
    m_ShapeRenderer.End();
}

void Shapes::OnImGuiRender()
{
    ImGui::Begin("AStar");
    if (ImGui::Button("Run"))
    {
        m_AStarResult.reset();
        AStar<Point> aStar;
        auto start = m_Points[0];
        auto end = m_Points[Random::Int(1, 99)];
        auto result = aStar.Search(start, end,
                [](const Point &a, const Point &b) -> float
                {
                    return glm::length(b.Position - a.Position);
                },
                [](const Point &a, const Point &b) -> float
                {
                    return glm::length(b.Position - a.Position);
                },
                [](const Point &point) -> std::vector<Point>
                {
                    std::vector<Point> neighbors;
                    for (auto &neighbor : point.Neighbors)
                    {
                        neighbors.push_back(*neighbor);
                    }
                    return neighbors;
                },
                [](const Point &a, const Point &b) -> bool
                {
                    return glm::all(glm::equal(a.Position, b.Position));
                }
        );
        m_AStarResult = std::make_unique<AStar<Point>::SearchResult>(result);
    }
    ImGui::End();
}

void Shapes::OnEvent(AppEvent event, void *eventData)
{

}
