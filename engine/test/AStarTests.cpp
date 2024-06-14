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

TEST(AStar, ShouldAllocateMemoryWhenSizeIsWithinLimit)
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
    });

    ASSERT_EQ(result.Path.size(), 2);
    ASSERT_EQ(result.Path.front()->UserData.Value, 1);
    ASSERT_EQ(result.Path.back()->UserData.Value, 2);
}
