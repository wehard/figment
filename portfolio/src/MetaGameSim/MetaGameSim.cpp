#include "MetaGameSim.h"
#include "AStar.h"

static uint32_t CashIncrease(uint32_t weaponLevel, uint32_t vehicleLevel)
{
    return weaponLevel * 5 + vehicleLevel * 10;
}

MetaGameSim::MetaGameSim(bool enabled) : Layer("MetaGameSim", enabled)
{
    ResetGameState();

    m_Actions.emplace_back(
            Action { .Name = Play, .Description = "Get Cash", .Function = [](GameState &state) -> GameState
            {
                auto newState = GameState(state);
                newState.Variables[Cash].Value += CashIncrease(newState.Variables[WeaponLevel].Value,
                        newState.Variables[VehicleLevel].Value);
                newState.Variables[Parts].Value +=
                        Random::Float() > 0.95 / newState.Variables[VehicleLevel].Value
                        ? newState.Variables[WeaponLevel].Value : 0;

                return newState;
            }});

    m_Actions.emplace_back(
            Action { .Name = BuyParts, .Description = "Buy Parts for Cash", .Function = [](
                    GameState &state) -> GameState
            {
                auto newState = GameState(state);
                if (newState.Variables[Cash].Value
                        < CashIncrease(newState.Variables[WeaponLevel].Value, newState.Variables[VehicleLevel].Value))
                    return newState;
                newState.Variables[Cash].Value -= CashIncrease(newState.Variables[WeaponLevel].Value,
                        newState.Variables[VehicleLevel].Value);
                newState.Variables[Parts].Value += 1;

                return newState;
            }});

    m_Actions.emplace_back(Action { .Name = UpgradeVehicle, .Description = "Upgrade Vehicle for Parts", .Function = [](
            GameState &state) -> GameState
    {
        auto newState = GameState(state);
        auto cost = 60 * newState.Variables[VehicleLevel].Value;
        if (newState.Variables[Parts].Value < cost)
            return newState;
        newState.Variables[Parts].Value -= cost;
        newState.Variables[VehicleLevel].Value += 1;

        return newState;
    }});

    m_Actions.emplace_back(Action { .Name = UpgradeWeapon, .Description = "Upgrade Weapon for Parts", .Function = [](
            GameState &state) -> GameState
    {
        auto newState = GameState(state);
        auto cost = 10 * state.Variables[WeaponLevel].Value;
        if (newState.Variables[Parts].Value < cost)
            return newState;
        newState.Variables[Parts].Value -= cost;
        newState.Variables[WeaponLevel].Value += 1;

        return newState;
    }});

}

void MetaGameSim::OnAttach() { }

void MetaGameSim::OnDetach() { }

void MetaGameSim::OnUpdate(float deltaTime) { }

void MetaGameSim::OnImGuiRender()
{
    auto size = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowSize(ImVec2(size.x * 0.60f, size.y * 0.60f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), ImGuiCond_Once, ImVec2(0.5, 0.5));
    ImGui::Begin(Title);
    {
        for (auto &action : m_Actions)
        {
            if (ImGui::Button(action.Name.c_str(), ImVec2(120, 20)))
            {
                m_GameState = action.Function(m_GameState);
                PushHistory(m_GameState, action.Name);
            }
            ImGui::SameLine();
        }

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 130);
        if (ImGui::Button("Reset", ImVec2(120, 20)))
        {
            ResetGameState();
            ResetSimulation();
        }

        ImGui::Separator();

        static AStar::SearchResult latestResult = {};
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2, 0.8, 0.2, 1.0));
        if (ImGui::Button("A*", ImVec2(120, 20)))
        {
            ResetGameState();
            ResetSimulation();

            AStar search(m_Actions);
            GameState start = GameState(m_GameState);
            GameState end = GameState(m_GameState);
            end.Variables[m_SimulationMaximiseGameVariable].Value = m_SimulationMaximiseGameVariableValue;
            auto result = search.Search(start, end, [this](const GameState &start, const GameState &end) -> uint32_t
            {
                return end.Variables.at(m_SimulationMaximiseGameVariable).Value
                        - start.Variables.at(m_SimulationMaximiseGameVariable).Value;
            });
            for (auto &actionState : result.Path)
            {
                m_GameState = actionState->GameState;
                PushHistory(m_GameState, actionState->ActionName);
            }
            latestResult = result;
        }
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::Text("Visited: %d", latestResult.VisitedCount);

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

        for (auto &[name, values] : m_GameHistory.VariableValues)
        {
            ImGui::PlotLines(name.c_str(), values.data(), (int)values.size(), 0,
                    nullptr, 0, m_GameHistory.VariableValueMax[name] + 2,
                    ImVec2(0, 30));
            ImGui::SameLine();
            ImGui::Text("%d", m_GameState.Variables[name].Value);
        }

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1, 0.1, 0.1, 1.0));
        ImGui::BeginChild("#actions");
        for (auto &action : m_GameHistory.ActionNames)
        {
            ImGui::Text("%s", action.c_str());
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::End();
    }
}

void MetaGameSim::OnEvent(AppEvent event, void *eventData) { }

void MetaGameSim::ResetGameState()
{
    m_GameState.Variables.clear();
    m_GameState.Variables[Cash] = { .Name = Cash, .Value = 0, .DefaultMaximizeValue = 1000 };
    m_GameState.Variables[Parts] = { .Name = Parts, .Value = 0, .DefaultMaximizeValue = 100 };
    m_GameState.Variables[WeaponLevel] = { .Name = WeaponLevel, .Value = 1, .DefaultMaximizeValue = 10 };
    m_GameState.Variables[VehicleLevel] = { .Name = VehicleLevel, .Value = 1, .DefaultMaximizeValue = 10 };
    m_GameHistory = GameHistory();
}

void MetaGameSim::ResetSimulation() { }

void MetaGameSim::PushHistory(GameState state, const std::string &actionName)
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
