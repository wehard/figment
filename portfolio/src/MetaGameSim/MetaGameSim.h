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
    struct GameVariable
    {
        std::string Name = "GameVariable";
        uint32_t Value = 0;
    };

    struct GameState
    {
        std::map<std::string, GameVariable> Variables;

        GameState() = default;
        GameState(const GameState &src)
        {
            for (const auto &[name, resource] : src.Variables)
            {
                Variables[name] = GameVariable { resource.Name, resource.Value };
            }
        }
    };

    struct GameHistory
    {
        std::vector<std::string> ActionNames = {};
        std::map<std::string, std::vector<float>> VariableValues = {};
        std::map<std::string, float> VariableValueMax = {};
    };

    struct Action
    {
        std::string Name;
        std::string Description;
        std::function<GameState(GameState &)> Function;
    };

private:
    void ResetGameState();
    void ResetSimulation();
    void PushHistory(GameState state, const std::string &actionName);

private:
    static constexpr const char *Title = "Meta Game Simulator";
    static constexpr const char *Play = "Play Game";
    static constexpr const char *BuyParts = "Buy Parts";
    static constexpr const char *UpgradeWeapon = "Upgrade Weapon";
    static constexpr const char *UpgradeVehicle = "Upgrade Vehicle";

    static constexpr const char *Cash = "Cash";
    static constexpr const char *Parts = "Parts";
    static constexpr const char *WeaponLevel = "Weapon Level";
    static constexpr const char *VehicleLevel = "Vehicle Level";

    GameState m_GameState;
    GameHistory m_GameHistory;
    std::vector<Action> m_Actions;

    std::string m_SimulationMaximiseGameVariable = "Cash";
    int m_SimulationMaximiseGameVariableValue = 1000;
};
