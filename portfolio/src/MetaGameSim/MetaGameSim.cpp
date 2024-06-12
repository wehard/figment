#include "MetaGameSim.h"
#include "AStar.h"

static uint32_t CashIncrease(uint32_t weaponLevel, uint32_t vehicleLevel)
{
    return weaponLevel * 5 + vehicleLevel * 10;
}

MetaGameSim::MetaGameSim(bool enabled) : Layer("MetaGameSim", enabled)
{
    ResetGameState();

    PushHistory(m_GameState, "Initial");

    m_Actions.emplace_back(
            Action { .Name = m_PlayLabel, .Description = "Play", .Function = [](GameState &state) -> GameState
            {
                auto newState = GameState(state);
                newState.Resources["Cash"].Amount += CashIncrease(newState.Items["Weapon"].Level,
                        newState.Items["Vehicle"].Level);
                newState.Resources["Parts"].Amount +=
                        Random::Float() > 0.95 / newState.Items["Vehicle"].Level ? newState.Items["Weapon"].Level : 0;

                return newState;
            }});

    m_Actions.emplace_back(
            Action { .Name = m_BuyPartsLabel, .Description = "Buy Parts", .Function = [](GameState &state) -> GameState
            {
                auto newState = GameState(state);
                if (newState.Resources["Cash"].Amount
                        < CashIncrease(newState.Items["Weapon"].Level, newState.Items["Vehicle"].Level))
                    return newState;
                newState.Resources["Cash"].Amount -= CashIncrease(newState.Items["Weapon"].Level,
                        newState.Items["Vehicle"].Level);
                newState.Resources["Parts"].Amount += 1;

                return newState;
            }});

    m_Actions.emplace_back(Action { .Name = m_UpgradeVehicleLabel, .Description = "Upgrade Vehicle", .Function = [](
            GameState &state) -> GameState
    {
        auto newState = GameState(state);
        auto cost = 60 * newState.Items["Vehicle"].Level;
        if (newState.Resources["Parts"].Amount < cost)
            return newState;
        newState.Resources["Parts"].Amount -= cost;
        newState.Items["Vehicle"].Level += 1;

        return newState;
    }});

    m_Actions.emplace_back(Action { .Name = m_UpgradeWeaponLabel, .Description = "Upgrade Weapon", .Function = [](
            GameState &state) -> GameState
    {
        auto newState = GameState(state);
        auto cost = 10 * state.Items["Weapon"].Level;
        if (newState.Resources["Parts"].Amount < cost)
            return newState;
        newState.Resources["Parts"].Amount -= cost;
        newState.Items["Weapon"].Level += 1;

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
    ImGui::Begin("MetaGameSim");

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
        end.Resources[m_SimulationMaximiseResource].Amount = m_SimulationMaximiseResourceAmount;
        auto result = search.Search(start, end, [this](const GameState &start, const GameState &end) -> uint32_t
        {
            return end.Resources.at(m_SimulationMaximiseResource).Amount
                    - start.Resources.at(m_SimulationMaximiseResource).Amount;
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
    ImGui::Text("Maximize Resource:");
    ImGui::SameLine();
    if (ImGui::Button(m_SimulationMaximiseResource.c_str(), ImVec2(120, 20)))
        ImGui::OpenPopup("#selectresource");

    if (ImGui::BeginPopup("#selectresource"))
    {
        for (auto &resource : m_GameState.Resources)
        {
            if (ImGui::MenuItem(resource.first.c_str()))
            {
                m_SimulationMaximiseResource = resource.first;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("Amount", &m_SimulationMaximiseResourceAmount, 1, 1);

    ImGui::Separator();

    for (auto &[name, values] : m_GameHistory.Resources)
    {
        ImGui::PlotLines(name.c_str(), values.data(), (int)values.size(), 0,
                nullptr, 0, m_GameHistory.ResourceMax[name] + 2,
                ImVec2(0, 30));
        ImGui::SameLine();
        ImGui::Text("%d", m_GameState.Resources[name].Amount);
    }

    for (auto &[name, values] : m_GameHistory.Items)
    {
        ImGui::PlotLines(name.c_str(), values.data(), (int)values.size(), 0,
                nullptr, 0, m_GameHistory.ItemMax[name] + 2,
                ImVec2(0, 30));
        ImGui::SameLine();
        ImGui::Text("%d", m_GameState.Items[name].Level);
    }

    if (!m_GameHistory.ActionNames.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1, 0.1, 0.1, 1.0));
        ImGui::BeginChild(1, ImVec2(0, 0));
        for (auto &action : m_GameHistory.ActionNames)
        {
            ImGui::Text("%s", action.c_str());
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    ImGui::End();
}

void MetaGameSim::OnEvent(AppEvent event, void *eventData) { }

void MetaGameSim::ResetGameState()
{
    m_GameState.Resources["Cash"].Amount = 0;
    m_GameState.Resources["Parts"].Amount = 0;
    m_GameState.Items["Weapon"].Level = 1;
    m_GameState.Items["Vehicle"].Level = 1;
    m_GameHistory = GameHistory();
}

void MetaGameSim::ResetSimulation() { }

void MetaGameSim::PushHistory(GameState state, const std::string &actionName)
{
    m_GameHistory.ActionNames.push_back(actionName);
    for (auto &[name, resource] : state.Resources)
    {
        m_GameHistory.Resources[name].push_back((float)resource.Amount);
        if ((float)resource.Amount > m_GameHistory.ResourceMax[name])
        {
            m_GameHistory.ResourceMax[name] = (float)resource.Amount;
        }
    }
    for (auto &[name, item] : state.Items)
    {
        m_GameHistory.Items[name].push_back((float)item.Level);
        if ((float)item.Level > m_GameHistory.ItemMax[name])
        {
            m_GameHistory.ItemMax[name] = (float)item.Level;
        }
    }
}
