#pragma once

#include "Figment.h"
#include <stack>

using namespace Figment;

class MetaGameSim : public Layer
{
public:
    explicit MetaGameSim(bool enabled);
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;

public:
    struct GameResource
    {
        std::string Name = "Resource";
        uint32_t Amount = 0;
    };

    struct GameItem
    {
        std::string Name = "Item";
        uint32_t Level = 0;
    };

    struct GameState
    {
        std::map<std::string, GameResource> Resources;
        std::map<std::string, GameItem> Items;

        GameState() = default;
        GameState(const GameState &src)
        {
            for (const auto &[name, resource] : src.Resources)
            {
                Resources[name] = GameResource { resource.Name, resource.Amount };
            }
            for (const auto &[name, item] : src.Items)
            {
                Items[name] = GameItem { item.Name, item.Level };
            }
        }
    };

    struct GameHistory
    {
        std::vector<std::string> ActionNames = {};
        std::map<std::string, std::vector<float>> Resources = {};
        std::map<std::string, std::vector<float>> Items = {};
        std::map<std::string, float> ResourceMax = {};
        std::map<std::string, float> ItemMax = {};
    };

    struct Action
    {
        std::string Name;
        std::string Description;
        std::function<GameState(GameState &)> Function;
    };

private:
    void SimulateStep(const std::string &resourceName);
    void ResetGameState();
    void ResetSimulation();
    void StartSimulation();
    void StopSimulation();
    void PushHistory(GameState state, const std::string &actionName);

private:
    const std::string m_Title = "Meta Game Simulator";
    const std::string m_PlayLabel = "Play Game";
    const std::string m_BuyPartsLabel = "Buy Parts";
    const std::string m_UpgradeWeaponLabel = "Upgrade Weapon";
    const std::string m_UpgradeVehicleLabel = "Upgrade Vehicle";

    std::string m_SimulationMaximiseResource = "Cash";
    int m_SimulationMaximiseResourceAmount = 1000;

    int m_MaxSimulationSteps = 10000;
    bool m_SimulationStarted = false;
    float m_SimulationStepInterval = 0.01f;
    float m_SimulationStepCounter = 0.0f;
    uint32_t m_SimulationStepCount = 0;

    GameState m_GameState;

    // std::map<std::string, std::function<GameState(GameState &)>> m_Actions;
    std::vector<Action> m_Actions;
    Action *m_NextAction = nullptr;

    GameHistory m_GameHistory;
};
