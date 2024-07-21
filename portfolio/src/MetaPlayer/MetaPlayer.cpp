#include "MetaPlayer.h"

MetaPlayer::MetaPlayer(bool enabled) : Layer("MetaPlayer", enabled)
{
    ResetGameState();
    InitializeActions();
}

static constexpr uint32_t CashIncreasePerWeaponLevel = 5;
static constexpr uint32_t CashIncreasePerVehicleLevel = 5;

static uint32_t GetPlayGameCashAmount(uint32_t weaponLevel, uint32_t vehicleLevel)
{
    return weaponLevel * CashIncreasePerWeaponLevel + vehicleLevel * CashIncreasePerVehicleLevel;
}

void MetaPlayer::InitializeActions()
{
    m_Actions.emplace_back(
            Action { .Name = Play, .Description = "Get Cash", .Function = [](const GameState &state) -> GameState
            {
                auto newState = GameState(state);
                newState.Variables[Cash].Value += GetPlayGameCashAmount(newState.Variables[WeaponLevel].Value,
                        newState.Variables[VehicleLevel].Value);
                return newState;
            }});

    m_Actions.emplace_back(
            Action { .Name = BuyParts, .Description = "Buy Parts for Cash", .Function = [](
                    const GameState &state) -> GameState
            {
                auto newState = GameState(state);
                newState.Variables[Parts].Value += state.GetValue(Cash) / PartsCost;
                newState.Variables[Cash].Value = state.GetValue(Cash) % PartsCost;

                return newState;
            }});

    m_Actions.emplace_back(Action { .Name = UpgradeWeapon, .Description = "Upgrade Weapon for Parts", .Function = [](
            const GameState &state) -> GameState
    {
        auto newState = GameState(state);
        auto partsCost = PartsBaseCostToIncrementWeaponLevel * newState.Variables[WeaponLevel].Value;
        if (newState.Variables[Parts].Value < partsCost)
            return state;
        newState.Variables[Parts].Value -= partsCost;
        newState.Variables[WeaponLevel].Value += 1;

        return newState;
    }});

    m_Actions.emplace_back(Action { .Name = UpgradeVehicle, .Description = "Upgrade Vehicle for Parts", .Function = [](
            const GameState &state) -> GameState
    {
        auto newState = GameState(state);
        auto partsCost = PartsBaseCostToIncrementVehicleLevel * newState.Variables[VehicleLevel].Value;
        if (newState.Variables[Parts].Value < partsCost)
            return newState;
        newState.Variables[Parts].Value -= partsCost;
        newState.Variables[VehicleLevel].Value += 1;

        return newState;
    }});
}

bool MetaPlayer::IsActionAvailable(const GameState &state, const std::string &actionName)
{
    if (actionName == Play)
        return true;
    if (actionName == BuyParts)
        return state.GetValue(Cash) >= GetPlayGameCashAmount(state.GetValue(WeaponLevel), state.GetValue(VehicleLevel));
    if (actionName == UpgradeWeapon)
        return state.GetValue(Parts) >= PartsBaseCostToIncrementWeaponLevel * state.GetValue(WeaponLevel);
    if (actionName == UpgradeVehicle)
        return state.GetValue(Parts) >= PartsBaseCostToIncrementVehicleLevel * state.GetValue(VehicleLevel);
    return false;
}

void MetaPlayer::StartSearch()
{
    ResetGameState();

    Figment::AStar<GameState> aStar;
    GameState start = GameState(m_GameState);
    start.ProducedByAction = "Start";
    GameState end = GameState(m_GameState);
    end.Variables[m_SimulationMaximiseGameVariable].Value = m_SimulationMaximiseGameVariableValue;
    end.ProducedByAction = "End";
    auto result = aStar.Search(start, end,
            [this](const GameState &start, const GameState &end) -> float
            {
                return (float)end.Variables.at(m_SimulationMaximiseGameVariable).Value
                        - (float)start.Variables.at(m_SimulationMaximiseGameVariable).Value;
            },
            [](const GameState &start, const GameState &end) -> float
            {
                return 1.0f;
            },
            [this](const GameState &state) -> std::vector<GameState>
            {
                std::vector<GameState> states;
                for (auto &action : m_Actions)
                {
                    if (!IsActionAvailable(state, action.Name))
                        continue;
                    auto newGameState = action.Function(state);
                    newGameState.ProducedByAction = action.Name;
                    states.push_back(newGameState);
                }
                return states;
            }, [](const GameState &a, const GameState &b) -> bool
            {
                // Check if the game state is the same.
                for (auto &variable : a.Variables)
                {
                    if (variable.second.Value != b.Variables.at(variable.first).Value)
                        return false;
                }
                return a.ProducedByAction == b.ProducedByAction;
            });
    for (auto &node : result.Path)
    {
        m_GameState = node->UserData;
        PushHistory(m_GameState, m_GameState.ProducedByAction);
    }
    m_AStarSearchResult = std::make_unique<AStar<GameState>::SearchResult>(result);
}

void MetaPlayer::OnImGuiRender()
{
    auto size = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowSize(ImVec2(size.x * 0.55f, size.y * 0.60f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), ImGuiCond_Once, ImVec2(0.5, 0.5));
    ImGui::Begin(m_Name.c_str());
    {
        for (auto &action : m_Actions)
        {
            ImGui::BeginDisabled(!IsActionAvailable(m_GameState, action.Name));
            if (ImGui::Button(action.Name.c_str(), ImVec2(120, 20)))
            {
                m_GameState = action.Function(m_GameState);
                PushHistory(m_GameState, action.Name);
            }
            ImGui::EndDisabled();
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("%s", action.Description.c_str());
            }
            ImGui::SameLine();
        }

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 130);
        if (ImGui::Button("Reset", ImVec2(120, 20)))
        {
            ResetGameState();
        }

        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2, 0.8, 0.2, 1.0));
        if (ImGui::Button("Start", ImVec2(120, 20)))
        {
            StartSearch();
        }
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::Text("Maximize:");
        ImGui::SameLine();
        if (ImGui::Button(m_SimulationMaximiseGameVariable.c_str(), ImVec2(120, 20)))
            ImGui::OpenPopup("#selectvariable");

        if (ImGui::BeginPopup("#selectvariable"))
        {
            for (auto &[name, variable] : m_GameState.Variables)
            {
                if (ImGui::MenuItem(name.c_str()))
                {
                    m_SimulationMaximiseGameVariable = name;
                    m_SimulationMaximiseGameVariableValue = (int)variable.DefaultMaximizeValue;
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::InputInt("Amount", &m_SimulationMaximiseGameVariableValue, 1, 1);

        ImGui::Separator();

        for (auto &[name, variable] : m_GameState.Variables)
        {
            std::vector<float> values = { (float)variable.Value, (float)variable.Value };
            if (m_GameHistory.VariableValues.find(name) != m_GameHistory.VariableValues.end())
                values = m_GameHistory.VariableValues[name];
            float max = 10;
            if (m_GameHistory.VariableValueMax.find(name) != m_GameHistory.VariableValueMax.end())
                max = m_GameHistory.VariableValueMax[name] + 2;
            ImGui::PlotLines(name.c_str(), values.data(), (int)values.size(), 0,
                    nullptr, 0, max,
                    ImVec2(0, 30));
            ImGui::SameLine();
            ImGui::Text("%d", m_GameState.Variables[name].Value);
        }

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1, 0.1, 0.1, 0.5));
        if (ImGui::BeginTable("Search Results", 2))
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Actions (%zu)", m_GameHistory.ActionNames.size());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Results");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::BeginChild("#actions");
            for (auto &action : m_GameHistory.ActionNames)
            {
                ImGui::Text("%s", action.c_str());
            }
            ImGui::EndChild();
            ImGui::TableSetColumnIndex(1);
            ImGui::BeginChild("#results");
            if (m_AStarSearchResult)
            {
                ImGui::Text("Nodes visited: %d", m_AStarSearchResult->NumVisited);
                ImGui::Text("Nodes evaluated: %d", m_AStarSearchResult->NumEvaluated);
                ImGui::Text("Paths discarded: %d", m_AStarSearchResult->NumPathsDiscarded);
            }
            ImGui::EndChild();
            ImGui::EndTable();
        }
        ImGui::PopStyleColor();

        ImGui::End();
    }
}

void MetaPlayer::ResetGameState()
{
    m_GameState.Variables.clear();
    m_GameState.Variables[Cash] = { .Name = Cash, .Value = 0, .DefaultMaximizeValue = DefaultCashMaximizeValue };
    m_GameState.Variables[Parts] = { .Name = Parts, .Value = 0, .DefaultMaximizeValue = DefaultPartsMaximizeValue };
    m_GameState.Variables[WeaponLevel] = { .Name = WeaponLevel, .Value = 1, .DefaultMaximizeValue = DefaultWeaponLevelMaximizeValue };
    m_GameState.Variables[VehicleLevel] = { .Name = VehicleLevel, .Value = 1, .DefaultMaximizeValue = DefaultVehicleLevelMaximizeValue };
    m_GameHistory = GameHistory();
    m_AStarSearchResult.reset();
}

void MetaPlayer::PushHistory(GameState state, const std::string &actionName)
{
    m_GameHistory.ActionNames.push_back(actionName);
    for (auto &[name, variable] : state.Variables)
    {
        m_GameHistory.VariableValues[name].push_back((float)variable.Value);
        if ((float)variable.Value > m_GameHistory.VariableValueMax[name])
        {
            m_GameHistory.VariableValueMax[name] = (float)variable.Value;
        }
    }
}
