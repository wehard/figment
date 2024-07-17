#include <gtest/gtest.h>
#include "AStar.h"
#include "Graph.h"
#include "../lib/glm/glm/glm.hpp"
#include "Random.h"

using namespace Figment;

struct NodeData
{
    int Value;

    bool operator==(const NodeData &other) const
    {
        return Value == other.Value;
    }
};

TEST(AStar, SearchReturnsEmptyPath)
{
    AStar<NodeData> aStar;

    auto result = aStar.Search({ .Value = 0 }, { .Value = 1 },
            [](const NodeData &start, const NodeData &end) -> int
            {
                return 0;
            },
            [](const NodeData &from, const NodeData &to) -> float
            {
                return 0;
            },
            [](const NodeData &state) -> std::vector<NodeData>
            {
                return {};
            },
            [](const NodeData &a, const NodeData &b) -> bool
            {
                return a.Value == b.Value;
            });

    ASSERT_EQ(result.Path.size(), 0);
}

TEST(AStar, SearchReturnsSinglePath)
{
    AStar<NodeData> aStar;

    auto result = aStar.Search({ .Value = 1 }, { .Value = 2 },
            [](const NodeData &start, const NodeData &end) -> int
            {
                return end.Value - start.Value;
            },
            [](const NodeData &from, const NodeData &to) -> float
            {
                return 1;
            },
            [](const NodeData &state) -> std::vector<NodeData>
            {
                std::vector<NodeData> states;
                states.push_back({ .Value = state.Value + 1 });
                return states;
            }, [](const NodeData &a, const NodeData &b) -> bool
            {
                return a.Value == b.Value;
            });

    ASSERT_EQ(result.Path.size(), 2);
    ASSERT_EQ(result.Path.front()->UserData.Value, 1);
    ASSERT_EQ(result.Path.back()->UserData.Value, 2);
}

struct Position
{
    float X;
    float Y;

    bool operator==(const Position &other) const
    {
        return X == other.X && Y == other.Y;
    }

    bool operator<(const Position &other) const
    {
        if (X < other.X)
            return true;
        if (X > other.X)
            return false;
        return Y < other.Y;
    }
};

static float Distance(const Position &a, const Position &b)
{
    return sqrtf(powf(b.X - a.X, 2) + powf(b.Y - a.Y, 2));
}

TEST(AStar, SearchReturnsOptimalPath)
{
    std::vector<Position> positions = {
            { .X = 0, .Y = 0 },
            { .X = 1, .Y = 0 },
            { .X = 0, .Y = 1 },
            { .X = 1, .Y = 1 },
    };

    std::map<Position, std::vector<int>> neighbourMap = {
            { Position { .X = 0, .Y = 0 }, std::vector<int> { 1, 2, 3 }},
            { Position { .X = 1, .Y = 0 }, std::vector<int> { 0, 2, 3 }},
            { Position { .X = 0, .Y = 1 }, std::vector<int> { 0, 1, 3 }},
            { Position { .X = 1, .Y = 1 }, std::vector<int> { 0, 1, 2 }},
    };

    AStar<Position> aStar;

    auto result = aStar.Search(positions.front(), positions.back(),
            [](const Position &start, const Position &end) -> float
            {
                return Distance(start, end);
            },
            [](const Position &from, const Position &to) -> float
            {
                return Distance(from, to);
            },
            [&positions, &neighbourMap](const Position &state) -> std::vector<Position>
            {
                std::vector<Position> neighbours;
                for (const auto &neighbour : neighbourMap[state])
                    neighbours.push_back(positions[neighbour]);
                return neighbours;
            }, [](const Position &a, const Position &b) -> bool
            {
                return a == b;
            });

    ASSERT_EQ(result.NumVisited, 1);
    ASSERT_EQ(result.NumEvaluated, 3);
    ASSERT_EQ(result.Path.size(), 2);
    ASSERT_EQ(result.Path.front()->UserData, positions.front());
    ASSERT_EQ(result.Path.back()->UserData, positions.back());
}

struct Point
{
    uint32_t Id;
    glm::vec3 Position;
    glm::vec4 Color;
    std::vector<Point *> Neighbors;
    bool Disabled = false;
};

static Graph<Point> CreateGraph(int width, int height)
{
    Graph<Point> graph;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            graph.AddNode(Point {
                    .Id = (uint32_t)(y * width + x),
                    .Position = { (float)x, (float)y, 0 },
                    .Color = { 0.8f, 0.2f, 0.3f, 1.0f },
                    .Disabled = false
            });
        }
    }

    for (auto &node : graph.GetNodes())
    {
        for (auto &neighbor : graph.GetNodes())
        {
            if (node.Id == neighbor.Id)
                continue;
            if (glm::all(glm::equal(node.Position, neighbor.Position))
                    || glm::length(neighbor.Position - node.Position) > 1.5f)
                continue;
            graph.AddEdge(&node, &neighbor);
        }
    }
    return graph;
}

TEST(AStar, SearchReturnsOptimalPathComplex)
{
    const uint32_t size = 4;
    AStar<Point> aStar;
    Graph<Point> graph = CreateGraph(size, size);

    auto &start = graph.GetNodes()[0];
    auto &end = graph.GetNodes()[size * size - 1];

    for (auto &i : graph.GetNodes())
    {
        i.Disabled = Random::Float() < 0.5f;
    }

    graph.GetNodes()[0].Disabled = false;
    graph.GetNodes()[size * size - 1].Disabled = false;

    // Print graph
    for (int y = size - 1; y >= 0; y--)
    {
        for (int x = 0; x < size; x++)
        {
            auto &node = graph.GetNodes()[y * size + x];
            // printf("%c", node.Disabled ? 178 : 176);
            printf("%s", node.Disabled ? "\u2591\u2591" : "\u2588\u2588");
        }
        printf("\n");
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
                return a.Id == b.Id;
            }
    );
    // printf("NumVisited: %d\n", result.NumVisited);
    // printf("NumEvaluated: %d\n", result.NumEvaluated);
    // printf("Path size: %d\n", result.Path.size());

    printf("Path: ");
    for (auto &node : result.Path)
    {
        printf("(%f, %f)", node->UserData.Position.x, node->UserData.Position.y);
        if (node != result.Path.back())
            printf(" -> ");
    }
    printf("\n");
}
