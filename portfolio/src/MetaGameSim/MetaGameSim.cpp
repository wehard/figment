#include "MetaGameSim.h"

// static void PushHistory(MetaGameSim::GameState &state, MetaGameSim::GameHistory &history, const std::string &actionName)
// {
//     history.ActionNames.push_back(actionName);
//     for (auto &resource : state.Resources)
//     {
//         history.Resources[resource.first].push_back((float)resource.second.Amount);
//         if ((float)resource.second.Amount > history.ResourceMax[resource.first])
//         {
//             history.ResourceMax[resource.first] = (float)resource.second.Amount;
//         }
//     }
//     for (auto &item : state.Items)
//     {
//         history.Items[item.first].push_back((float)item.second.Level);
//         if ((float)item.second.Level > history.ItemMax[item.first])
//         {
//             history.ItemMax[item.first] = (float)item.second.Level;
//         }
//     }
// }

static uint32_t CashIncrease(uint32_t weaponLevel, uint32_t vehicleLevel)
{
    return weaponLevel * 5 + vehicleLevel * 10;
}

MetaGameSim::MetaGameSim(bool enabled) : Layer("MetaGameSim", enabled)
{
    ResetGameState();

    m_Actions[m_PlayLabel] = [this](GameState &state) -> GameState
    {
        auto newState = GameState(state);
        newState.Resources["Cash"].Amount += CashIncrease(newState.Items["Weapon"].Level,
                newState.Items["Vehicle"].Level);
        newState.Resources["Parts"].Amount +=
                Random::Float() > 0.95 / newState.Items["Vehicle"].Level ? newState.Items["Weapon"].Level : 0;

        PushHistory(newState, m_PlayLabel);
        return newState;
    };

    m_Actions[m_BuyPartsLabel] = [this](GameState &state) -> GameState
    {
        auto newState = GameState(state);
        if (newState.Resources["Cash"].Amount
                < CashIncrease(newState.Items["Weapon"].Level, newState.Items["Vehicle"].Level))
            return newState;
        newState.Resources["Cash"].Amount -= CashIncrease(newState.Items["Weapon"].Level,
                newState.Items["Vehicle"].Level);
        newState.Resources["Parts"].Amount += 1;

        PushHistory(newState, m_BuyPartsLabel);
        return newState;
    };

    m_Actions[m_UpgradeWeaponLabel] = [this](GameState &state) -> GameState
    {
        auto newState = GameState(state);
        auto cost = 10 * state.Items["Weapon"].Level;
        if (newState.Resources["Parts"].Amount < cost)
            return newState;
        newState.Resources["Parts"].Amount -= cost;
        newState.Items["Weapon"].Level += 1;

        PushHistory(newState, m_UpgradeWeaponLabel);
        return newState;
    };

    m_Actions[m_UpgradeVehicleLabel] = [this](GameState &state) -> GameState
    {
        auto newState = GameState(state);
        auto cost = 60 * newState.Items["Vehicle"].Level;
        if (newState.Resources["Parts"].Amount < cost)
            return newState;
        newState.Resources["Parts"].Amount -= cost;
        newState.Items["Vehicle"].Level += 1;

        PushHistory(newState, m_UpgradeVehicleLabel);
        return newState;
    };
}

void MetaGameSim::OnAttach()
{
}

void MetaGameSim::OnDetach()
{
}

void MetaGameSim::OnUpdate(float deltaTime)
{
    if (m_SimulationStepCount >= m_MaxSimulationSteps)
    {
        StopSimulation();
    }
    if (!m_SimulationStarted)
        return;
    if (m_SimulationStepCounter >= m_SimulationStepInterval
            && m_SimulationStepCount <= m_MaxSimulationSteps)
    {
        SimulateStep(m_SimulationMaximiseResource);
        if (!m_NextAction.empty())
        {
            auto Action = m_Actions[m_NextAction];
            FIG_LOG_INFO("Simulated action: %s", m_NextAction.c_str());
            m_GameState = Action(m_GameState);
            m_NextAction.clear();
        }
        m_SimulationStepCounter = 0.0f;
    }
    m_SimulationStepCounter += deltaTime;
}

void MetaGameSim::OnImGuiRender()
{
    auto size = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowSize(ImVec2(size.x * 0.60f, size.y * 0.60f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), ImGuiCond_Once, ImVec2(0.5, 0.5));
    ImGui::Begin("MetaGameSim");

    for (auto &action : m_Actions)
    {
        ImGui::SameLine();
        if (ImGui::Button(action.first.c_str(), ImVec2(120, 20)))
        {
            action.second(m_GameState);
        }
    }
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2, 0.8, 0.2, 1.0));
    if (ImGui::Button("Simulate", ImVec2(120, 20)))
    {
        ResetGameState();
        ResetSimulation();
        StartSimulation();
    }
    ImGui::PopStyleColor();

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

    if (m_SimulationStarted)
    {
        ImGui::SameLine();
        ImGui::Text("Simulation Step: %d", m_SimulationStepCount);
    }

    ImGui::Separator();

    // for (auto &history : m_GameHistory)
    // {
    //     ImGui::PlotLines(resource.first.c_str(), resource.second.data(), (int)resource.second.size(), 0,
    //             nullptr, 0, m_GameHistory.ResourceMax[resource.first] + 2,
    //             ImVec2(0, 30));
    //     ImGui::SameLine();
    //     ImGui::Text("%d", m_GameState.Resources[resource.first].Amount);
    // }
    //
    // for (auto &item : m_GameHistory.Items)
    // {
    //     ImGui::PlotLines(item.first.c_str(), item.second.data(), (int)item.second.size(), 0,
    //             nullptr, 0, m_GameHistory.ItemMax[item.first] + 2,
    //             ImVec2(0, 30));
    //     ImGui::SameLine();
    //     ImGui::Text("%d", m_GameState.Items[item.first].Level);
    // }

    for (auto &history : m_ResourceHistory)
    {
        ImGui::Text("%f", history);
    }

    ImGui::End();
}

void MetaGameSim::OnEvent(AppEvent event, void *eventData)
{
}

static bool IsResourceAvailable(const MetaGameSim::GameState &state, const std::string &resourceName, uint32_t amount)
{
    for (const auto &resource : state.Resources)
    {
        if (resource.first == resourceName)
        {
            return resource.second.Amount >= amount;
        }
    }
    return false;
}

static MetaGameSim::GameState TryAction(MetaGameSim::GameState &state,
        const std::function<MetaGameSim::GameState(MetaGameSim::GameState &)> &action)
{
    auto newState = action(state);
    return newState;
}

void MetaGameSim::SimulateStep(const std::string &resourceName)
{

    GameState bestState = GameState(m_GameState);
    std::string bestActionName;
    for (auto &action : m_Actions)
    {
        auto newState = TryAction(m_GameState, action.second);
        if (newState.Resources[resourceName].Amount > bestState.Resources[resourceName].Amount)
        {
            FIG_LOG_INFO("New best state: %d, Action = %s", newState.Resources[resourceName].Amount,
                    action.first.c_str());
            bestState = GameState(newState);
            bestActionName = action.first;
        }
    }
    m_NextAction = bestActionName;
    m_SimulationStepCount++;
}

void MetaGameSim::ResetGameState()
{
    m_GameState.Resources["Cash"].Amount = 0;
    m_GameState.Resources["Parts"].Amount = 0;
    m_GameState.Items["Weapon"].Level = 1;
    m_GameState.Items["Vehicle"].Level = 1;
    m_GameHistory.clear();
}

void MetaGameSim::ResetSimulation()
{
    m_SimulationStepCounter = 0.0f;
    m_SimulationStepCount = 0;
    m_NextAction.clear();
}

void MetaGameSim::StartSimulation()
{
    m_SimulationStarted = true;
}

void MetaGameSim::StopSimulation()
{
    m_SimulationStarted = false;
}

void MetaGameSim::PushHistory(GameState state, const std::string &actionName)
{
    m_ResourceHistory.push_back((float)state.Resources[m_SimulationMaximiseResource].Amount);
    m_GameHistory.emplace_back(actionName, state);
}