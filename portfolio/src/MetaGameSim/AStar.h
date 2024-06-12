#pragma once

#include "MetaGameSim.h"
#include <queue>

struct ActionState
{
    std::string ActionName;
    MetaGameSim::GameState GameState;
    int GScore = 0; // Steps taken to reach this state
    int HScore = 0; // Estimated steps to reach the end state
    std::shared_ptr<ActionState> parent = nullptr;
    [[nodiscard]] uint32_t FScore() const
    {
        return GScore + HScore;
    };
};

class AStar
{
public:
    struct SearchResult
    {
        std::vector<std::shared_ptr<ActionState>> Path;
        uint32_t VisitedCount = 0;
    };

public:
    explicit AStar(const std::vector<MetaGameSim::Action> &actions) : m_Actions(actions) { }

    SearchResult Search(const MetaGameSim::GameState &startState,
            const MetaGameSim::GameState &endState,
            const std::function<int(const MetaGameSim::GameState &,
                    const MetaGameSim::GameState &)> &calculateHScore)
    {
        auto comp = [](const std::shared_ptr<ActionState> &a, const std::shared_ptr<ActionState> &b)
        { return a->FScore() < b->FScore(); };
        std::multiset<std::shared_ptr<ActionState>, decltype(comp)> openSet(comp);
        std::vector<std::shared_ptr<ActionState>> closedSet;

        auto endActionState = std::make_shared<ActionState>(ActionState {
                .ActionName = "End",
                .GameState = endState,
                .GScore = 0,
                .HScore = 0 });

        auto startActionState = std::make_shared<ActionState>(ActionState {
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
                auto newActionState = std::make_shared<ActionState>(ActionState {
                        .ActionName = action.Name,
                        .GameState = newGameState,
                        .GScore = currentState->GScore + 1,
                        .HScore = calculateHScore(newGameState, endState),
                        .parent = currentState
                });

                auto it = std::find_if(openSet.begin(), openSet.end(),
                        [calculateHScore, &newActionState](const std::shared_ptr<ActionState> &as)
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
        std::vector<std::shared_ptr<ActionState>> path;
        std::shared_ptr<ActionState> state = endActionState;
        while (state != nullptr)
        {
            path.push_back(state);
            state = state->parent;
        }
        std::reverse(path.begin(), path.end());

        return { path, visitedCount };
    }

private:
    const std::vector<MetaGameSim::Action> &m_Actions;
};
