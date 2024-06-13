#pragma once

#include "MetaPlayer.h"
#include <queue>

class AStar
{
public:
    struct Node
    {
        std::string ActionName;
        MetaPlayer::GameState GameState;
        int GScore = 0; // Steps taken to reach this state
        int HScore = 0; // Estimated steps to reach the end state
        std::shared_ptr<Node> parent = nullptr;
        [[nodiscard]] uint32_t FScore() const
        {
            return GScore + HScore;
        };
    };

    struct SearchResult
    {
        std::vector<std::shared_ptr<Node>> Path;
        uint32_t VisitedCount = 0;
    };

public:
    explicit AStar(const std::vector<MetaPlayer::Action> &actions) : m_Actions(actions) { }

    SearchResult Search(const MetaPlayer::GameState &startState,
            const MetaPlayer::GameState &endState,
            const std::function<int(const MetaPlayer::GameState &,
                    const MetaPlayer::GameState &)> &calculateHScore)
    {
        auto comp = [](const std::shared_ptr<Node> &a, const std::shared_ptr<Node> &b)
        { return a->FScore() < b->FScore(); };
        std::multiset<std::shared_ptr<Node>, decltype(comp)> openSet(comp);
        std::vector<std::shared_ptr<Node>> closedSet;

        auto endActionState = std::make_shared<Node>(Node {
                .ActionName = "End",
                .GameState = endState,
                .GScore = 0,
                .HScore = 0 });

        auto startActionState = std::make_shared<Node>(Node {
                .ActionName = "Start",
                .GameState = startState,
                .GScore = 0,
                .HScore = calculateHScore(startState, endState)
        });
        openSet.insert(startActionState);

        uint32_t visitedCount = 0;

        while (!openSet.empty())
        {
            auto currentState = *openSet.begin();
            openSet.erase(openSet.begin());

            if (currentState->HScore <= endActionState->HScore)
            {
                endActionState = currentState;
                break;
            }

            for (auto &action : m_Actions)
            {
                auto newGameState = action.Function(currentState->GameState);
                auto newActionState = std::make_shared<Node>(Node {
                        .ActionName = action.Name,
                        .GameState = newGameState,
                        .GScore = currentState->GScore + 1,
                        .HScore = calculateHScore(newGameState, endState),
                        .parent = currentState
                });

                auto it = std::find_if(openSet.begin(), openSet.end(),
                        [calculateHScore, &newActionState](const std::shared_ptr<Node> &as)
                        {
                            return as->FScore() == newActionState->FScore();
                        });

                if (it != openSet.end())
                {
                    if (newActionState->FScore() < it->get()->FScore())
                    {
                        openSet.erase(it);
                        openSet.insert(newActionState);
                    }
                }
                else
                {
                    openSet.insert(newActionState);
                }
                visitedCount++;
            }
            closedSet.push_back(currentState);
        }
        std::vector<std::shared_ptr<Node>> path;
        std::shared_ptr<Node> state = endActionState;
        while (state != nullptr)
        {
            path.push_back(state);
            state = state->parent;
        }
        std::reverse(path.begin(), path.end());

        return { path, visitedCount };
    }

private:
    const std::vector<MetaPlayer::Action> &m_Actions;
};
