#pragma once

#include "Figment.h"
#include <stack>

using namespace Figment;

class MetaPlayer : public Layer
{
public:
    explicit MetaPlayer(bool enabled);
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
        uint32_t DefaultMaximizeValue = 0;
    };

    struct GameState
    {
        std::map<std::string, GameVariable> Variables;
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
    static constexpr const char *Play = "Play Game";
    static constexpr const char *BuyParts = "Buy Parts";
    static constexpr const char *UpgradeWeapon = "Upgrade Weapon";
    static constexpr const char *UpgradeVehicle = "Upgrade Vehicle";

    static constexpr const char *Cash = "Cash";
    static constexpr const char *Parts = "Parts";
    static constexpr const char *WeaponLevel = "Weapon Level";
    static constexpr const char *VehicleLevel = "Vehicle Level";

    static constexpr uint32_t DefaultCashMaximizeValue = 1000;
    static constexpr uint32_t DefaultPartsMaximizeValue = 100;
    static constexpr uint32_t DefaultWeaponLevelMaximizeValue = 10;
    static constexpr uint32_t DefaultVehicleLevelMaximizeValue = 10;

    GameState m_GameState;
    GameHistory m_GameHistory;
    std::vector<Action> m_Actions;

    std::string m_SimulationMaximiseGameVariable = "Cash";
    int m_SimulationMaximiseGameVariableValue = DefaultCashMaximizeValue;
};