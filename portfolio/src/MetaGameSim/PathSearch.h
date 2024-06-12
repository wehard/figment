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

class PathSearch
{
public:

    explicit PathSearch(const std::vector<MetaGameSim::Action> &actions) : m_Actions(actions) { }

    std::vector<std::shared_ptr<ActionState>> AStar(const MetaGameSim::GameState &startState,
            const MetaGameSim::GameState &endState,
            const std::function<int(const MetaGameSim::GameState &,
                    const MetaGameSim::GameState &)> &calculateHCost,
            uint32_t maxIterations)
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
                .HScore = calculateHCost(startState,
                        endState) });
        openSet.insert(startActionState);

        uint32_t iterations = 0;

        while (!openSet.empty() && iterations++ < maxIterations)
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
                auto newState = action.Function(currentState->GameState);
                auto newActionState = std::make_shared<ActionState>(ActionState {
                        .ActionName = action.Name,
                        .GameState = newState,
                        .GScore = currentState->GScore + 1,
                        .HScore = calculateHCost(newState, endState),
                        .parent = currentState
                });

                auto it = std::find_if(openSet.begin(), openSet.end(),
                        [calculateHCost, &newActionState](const std::shared_ptr<ActionState> &as)
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
        return path;
    }

private:
    const std::vector<MetaGameSim::Action> &m_Actions;
};
