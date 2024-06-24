#pragma once

#include <set>

namespace Figment
{
    template<typename NodeUserData>
    class AStar
    {
    public:
        struct Node
        {
            NodeUserData UserData;
            float GScore = 0; // Steps taken to reach this node
            float HScore = 0; // Estimated steps to reach end node
            std::shared_ptr<Node> parent = nullptr;
            [[nodiscard]] float FScore() const
            {
                return GScore + HScore;
            };
        };

        struct SearchResult
        {
            std::vector<std::shared_ptr<Node>> Path;
            uint32_t NumVisited = 0;
            uint32_t NumEvaluated = 0;
            uint32_t NumPathsDiscarded = 0;
        };

    public:
        AStar() = default;
        ~AStar() = default;

        SearchResult Search(const NodeUserData &start, const NodeUserData &end,
                const std::function<float(const NodeUserData &, const NodeUserData &)> &calculateHScore,
                const std::function<float(const NodeUserData &, const NodeUserData &)> &edgeCost,
                const std::function<std::vector<NodeUserData>(const NodeUserData &)> &getNeighbors,
                const std::function<bool(const NodeUserData &a, const NodeUserData &b)> &equals)
        {
            auto comp = [](const std::shared_ptr<Node> &a, const std::shared_ptr<Node> &b)
            { return a->FScore() < b->FScore(); };
            std::multiset<std::shared_ptr<Node>, decltype(comp)> openSet(comp);
            std::vector<std::shared_ptr<Node>> closedSet;

            auto startNode = std::make_shared<Node>(Node {
                    .UserData = start,
                    .GScore = 0,
                    .HScore = calculateHScore(start, end)
            });

            auto endNode = std::make_shared<Node>(Node {
                    .UserData = end,
                    .GScore = 0,
                    .HScore = 0
            });

            openSet.insert(startNode);

            uint32_t numEvaluated = 0;
            uint32_t numPathsDiscarded = 0;
            while (!openSet.empty())
            {
                auto currentNode = *openSet.begin();
                openSet.erase(openSet.begin());

                if (equals(currentNode->UserData, endNode->UserData))
                {
                    std::vector<std::shared_ptr<Node>> path;
                    auto node = currentNode;
                    while (node != nullptr)
                    {
                        path.push_back(node);
                        node = node->parent;
                    }
                    std::reverse(path.begin(), path.end());
                    return SearchResult { .Path = path, .NumVisited = (uint32_t)closedSet.size(), .NumEvaluated = numEvaluated, .NumPathsDiscarded = numPathsDiscarded };
                }

                auto neighbors = getNeighbors(currentNode->UserData);
                for (const auto &neighbor : neighbors)
                {
                    auto gScore = currentNode->GScore
                            + edgeCost(currentNode->UserData, neighbor);
                    auto hScore = calculateHScore(neighbor, end);
                    auto neighborNode = std::make_shared<Node>(Node {
                            .UserData = neighbor,
                            .GScore = gScore,
                            .HScore = hScore
                    });

                    auto it = std::find_if(openSet.begin(), openSet.end(),
                            [&equals, &neighborNode](const std::shared_ptr<Node> &node)
                            {
                                return equals(node->UserData, neighborNode->UserData);
                            });

                    if (it != openSet.end())
                    {
                        // If the new path is shorter, update the GScore and parent
                        if (gScore < (*it)->GScore)
                        {
                            (*it)->GScore = gScore;
                            (*it)->parent = currentNode;
                            numPathsDiscarded++;
                        }
                    }
                    else
                    {
                        // If the neighbor is not in the open set, add it
                        neighborNode->parent = currentNode;
                        openSet.insert(neighborNode);
                    }
                    numEvaluated++;
                }

                closedSet.push_back(currentNode);
            }
            return SearchResult { .Path = {}, .NumVisited = (uint32_t)closedSet.size(), .NumEvaluated = numEvaluated, .NumPathsDiscarded = numPathsDiscarded };
        }
    };

}
