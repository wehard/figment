#include <gtest/gtest.h>
#include "AStar.h"

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

    auto result = aStar.Search({ .Value = 0 }, { .Value = 1 }, [](const NodeData &start, const NodeData &end) -> int
    {
        return 0;
    }, [](const NodeData &state) -> std::vector<NodeData>
    {
        return {};
    }, [](const NodeData &a, const NodeData &b) -> bool
    {
        return a.Value == b.Value;
    });

    ASSERT_EQ(result.Path.size(), 0);
}

TEST(AStar, SearchReturnsSinglePath)
{
    AStar<NodeData> aStar;

    auto result = aStar.Search({ .Value = 1 }, { .Value = 2 }, [](const NodeData &start, const NodeData &end) -> int
    {
        return end.Value - start.Value;
    }, [](const NodeData &state) -> std::vector<NodeData>
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

TEST(AStar, SearchReturnsOptimalPath)
{
    struct Position
    {
        int X;
        int Y;

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
            [](const Position &start, const Position &end) -> int
            {
                return abs(end.X - start.X) + abs(end.Y - start.Y);
            }, [&positions, &neighbourMap](const Position &state) -> std::vector<Position>
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
    ASSERT_EQ(result.Path.size(), 2);
    ASSERT_EQ(result.Path.front()->UserData, positions.front());
    ASSERT_EQ(result.Path.back()->UserData, positions.back());
}
