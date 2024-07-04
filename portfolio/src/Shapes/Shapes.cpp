#include "Shapes.h"
#include "AStar.h"

Shapes::Shapes(WebGPUContext &context) : Layer("Shapes", true), m_Context(context),
        m_ShapeRenderer(context)
{
    m_Camera = OrthographicCamera((float)context.GetSwapChainWidth(), (float)context.GetSwapChainHeight());
    m_Camera.Zoom(60.0f, { context.GetSwapChainWidth() / 2, context.GetSwapChainHeight() / 2 });
    m_Camera.SetPosition({ 9.5, 4.5, 0 });

    const int width = 20;
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
                            .Color = { 0.8f, 0.2f, 0.3f, 1.0f },
                            .Disabled = false

                    };
        }
    }

    m_Points[0].Color = { 0.8, 0.5, 0.2, 1 };
    m_Points[m_Points.size() - 1].Color = { 0.2, 0.6, 0.8, 1 };

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (x < width - 1)
            {
                m_Points[x + y * width].Neighbors.push_back(&m_Points[x + 1 + y * width]);
            }
            if (y < height - 1)
            {
                m_Points[x + y * width].Neighbors.push_back(&m_Points[x + (y + 1) * width]);
            }
            if (x < width - 1 && y < height - 1)
            {
                m_Points[x + y * width].Neighbors.push_back(&m_Points[x + 1 + (y + 1) * width]);
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
    m_TimeSinceLastRun += deltaTime;
    if (RunningAStar && m_TimeSinceLastRun >= m_RunAStarInterval)
    {
        RunAStar();
        m_TimeSinceLastRun = 0.0f;
    }

    static glm::vec3 offset = { -4.5f, -4.5f, 0.0f };
    m_ShapeRenderer.Begin(m_Camera);
    for (auto &point : m_Points)
    {
        auto color = point.Disabled ? glm::vec4(0.2f, 0.2f, 0.2f, 1.0f) : point.Color;
        m_ShapeRenderer.SubmitCircle(point.Position, color, 0.5, -1);
    }
    if (m_AStarResult && m_AStarResult->Path.size() > 1)
    {
        for (size_t i = 0; i < m_AStarResult->Path.size() - 1; i++)
        {
            m_ShapeRenderer.SubmitLine(m_AStarResult->Path[i]->UserData.Position,
                    m_AStarResult->Path[i + 1]->UserData.Position, { 1, 1, 1, 1 }, -1);
        }
    }
    m_ShapeRenderer.End();
}

void Shapes::OnImGuiRender()
{
}

void Shapes::OnEvent(AppEvent event, void *eventData)
{

}

void Shapes::RunAStar()
{
    m_AStarResult.reset();
    AStar<Point> aStar;
    auto start = m_Points[0];
    auto end = m_Points[m_Points.size() - 1];

    for (int i = 1; i < m_Points.size() - 1; i++)
    {
        m_Points[i].Disabled = Random::Float() < 0.2;
    }

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
                    if (!neighbor->Disabled)
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
