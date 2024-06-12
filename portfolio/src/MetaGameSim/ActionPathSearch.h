#pragma once

#include "MetaGameSim.h"
#include <queue>

struct ActionState
{
    std::string ActionName;
    MetaGameSim::GameState GameState;
    uint32_t Cost;
};

bool operator<(const MetaGameSim::GameState &a, const MetaGameSim::GameState &b)
{
    return a.Resources.at("Cash").Amount < b.Resources.at("Cash").Amount;
}

bool operator==(const MetaGameSim::GameState &a, const MetaGameSim::GameState &b)
{
    return a.Resources.at("Cash").Amount == b.Resources.at("Cash").Amount;
}

bool operator<(const ActionState &a, const ActionState &b)
{
    return a.GameState.Resources.at("Cash").Amount < b.GameState.Resources.at("Cash").Amount;
}

bool operator==(const ActionState &a, const ActionState &b)
{
    return a.GameState.Resources.at("Cash").Amount == b.GameState.Resources.at("Cash").Amount;
}

bool operator>=(const ActionState &a, const ActionState &b)
{
    return a.GameState.Resources.at("Cash").Amount >= b.GameState.Resources.at("Cash").Amount;
}

class ActionPathSearch
{
public:

    explicit ActionPathSearch(const std::vector<MetaGameSim::Action> &actions) : m_Actions(actions) { }

    static uint32_t CalculateCost(const MetaGameSim::GameState &state, const MetaGameSim::GameState &endState)
    {
        return endState.Resources.at("Cash").Amount - state.Resources.at("Cash").Amount;
    }

    std::vector<ActionState> AStar(const MetaGameSim::GameState &startState, const MetaGameSim::GameState &endState,
            uint32_t maxIterations)
    {
        std::priority_queue<ActionState> openSet;
        std::vector<ActionState> closedSet;

        ActionState endActionState = { .ActionName = "End", .GameState = endState, .Cost = 1000 };

        openSet.push(ActionState { .ActionName = "Start", .GameState = startState, .Cost = CalculateCost(startState,
                endState) });

        uint32_t iterations = 0;

        while (!openSet.empty() && iterations++ < maxIterations)
        {
            auto currentState = openSet.top();
            openSet.pop();

            if (currentState >= endActionState)
            {
                break;
            }

            for (auto &action : m_Actions)
            {
                auto newState = action.Function(currentState.GameState);
                ActionState newActionState = { .ActionName = action.Name, .GameState = newState, .Cost = CalculateCost(
                        newState,
                        endState) };
                if (std::find(closedSet.begin(), closedSet.end(), newActionState) != closedSet.end())
                {
                    continue;
                }

                openSet.push(newActionState);
            }
            closedSet.push_back(currentState);
        }
        // for (auto &state : closedSet)
        // {
        //     Log::Info("State: Action = %s, Cash = %d", state.ActionName.c_str(),
        //             state.GameState.Resources.at("Cash").Amount);
        // }
        return closedSet;
    }

private:
    const std::vector<MetaGameSim::Action> &m_Actions;
};
