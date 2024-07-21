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
        std::string ProducedByAction;
        std::map<std::string, GameVariable> Variables;

        /// Returns the value of the variable with the given name.
        /// @param name The name of the variable.
        /// @return The value of the variable.
        [[nodiscard]] uint32_t GetValue(const std::string &name) const
        {
            return Variables.at(name).Value;
        }

        void SetValue(const std::string &name, uint32_t value)
        {
            Variables[name].Value = value;
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
        std::function<GameState(const GameState &)> Function;
    };

public:
    explicit MetaPlayer(bool enabled);
    void OnAttach() override { };
    void OnDetach() override { };
    void OnEnable() override { };
    void OnDisable() override { };
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
    static constexpr uint32_t DefaultWeaponLevelMaximizeValue = 4;
    static constexpr uint32_t DefaultVehicleLevelMaximizeValue = 2;

    static constexpr uint32_t PartsCost = 10;
    static constexpr uint32_t PartsBaseCostToIncrementWeaponLevel = 10;
    static constexpr uint32_t PartsBaseCostToIncrementVehicleLevel = 100;

    GameState m_GameState;
    GameHistory m_GameHistory;
    std::vector<Action> m_Actions;

    std::unique_ptr<AStar<GameState>::SearchResult> m_AStarSearchResult;
    std::string m_SimulationMaximiseGameVariable = Cash;
    int m_SimulationMaximiseGameVariableValue = DefaultCashMaximizeValue;
};
