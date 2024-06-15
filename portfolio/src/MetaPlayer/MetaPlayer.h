#pragma once

#include "Figment.h"
#include "AStar.h"
#include <stack>

using namespace Figment;

class MetaPlayer : public Layer
{
public:
    struct GameVariable
    {
        std::string Name = "GameVariable";
        uint32_t Value = 0;
        uint32_t DefaultMaximizeValue = 0;
    };

    struct GameState
    {
        std::string ActionName;
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
        std::function<GameState(const GameState &)> Function;
    };

public:
    explicit MetaPlayer(bool enabled);
    void OnAttach() override { };
    void OnDetach() override { };
    void OnUpdate(float deltaTime) override { };
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override { };

private:
    void InitializeActions();
    static bool IsActionAvailable(const GameState &state, const std::string &actionName);
    void StartSearch();
    void ResetGameState();
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
    static constexpr uint32_t DefaultWeaponLevelMaximizeValue = 5;
    static constexpr uint32_t DefaultVehicleLevelMaximizeValue = 5;

    GameState m_GameState;
    GameHistory m_GameHistory;
    std::vector<Action> m_Actions;

    std::unique_ptr<AStar<GameState>::SearchResult> m_AStarSearchResult;
    std::string m_SimulationMaximiseGameVariable = Cash;
    int m_SimulationMaximiseGameVariableValue = DefaultCashMaximizeValue;
};
