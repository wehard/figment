#include "MetaGameSim.h"

static void PushHistory(MetaGameSim::GameState &state, MetaGameSim::GameHistory &history)
{
    for (auto &resource : state.Resources)
    {
        history.Resources[resource.Name].push_back((float)resource.Amount);
    }
}

MetaGameSim::MetaGameSim(bool enabled) : Layer("MetaGameSim", enabled)
{
    m_GameState.Resources.emplace_back(GameResource { "Gold", 100 });
    m_GameState.Resources.emplace_back(GameResource { "Gem", 0 });
    m_GameState.Items.emplace_back(GameItem { "Weapon", 1 });
    m_GameState.Items.emplace_back(GameItem { "Vehicle", 1 });

    m_Actions.emplace_back(Action { "Play Core Game", "Gives\n\t10 Gold\n\tRandom Gem", [this](GameState &state)
    {
        state.Resources[0].Amount += 10;
        state.Resources[1].Amount += Random::Float() > 0.95 ? 1 : 0;

        PushHistory(state, m_GameHistory);
    }});

    m_Actions.emplace_back(Action { "Buy Gem", "", [this](GameState &state)
    {
        if (state.Resources[0].Amount < 10)
            return;
        state.Resources[0].Amount -= 10;
        state.Resources[1].Amount += 1;

        PushHistory(state, m_GameHistory);
    }});

    m_Actions.emplace_back(Action { "Upgrade Weapon", "", [this](GameState &state)
    {
        if (state.Resources[1].Amount < 10)
            return;
        state.Resources[1].Amount -= 10;
        state.Items[0].Level += 1;

        PushHistory(state, m_GameHistory);
    }});
}

void MetaGameSim::OnAttach()
{
}

void MetaGameSim::OnDetach()
{
}

void MetaGameSim::OnUpdate(float deltaTime)
{
}

void MetaGameSim::OnImGuiRender()
{
    auto size = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(ImVec2(size.x / 2, size.y / 3), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), ImGuiCond_Once, ImVec2(0.5, 0.5));
    ImGui::Begin("MetaGameSim");

    for (auto &action : m_Actions)
    {
        if (ImGui::Button(action.Name.c_str()))
        {
            action.Function(m_GameState);
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("%s", action.Description.c_str());
        }

    }

    ImGui::Separator();

    for (auto &resource : m_GameState.Resources)
    {
        ImGui::Text("%s: %d", resource.Name.c_str(), resource.Amount);
    }
    for (auto &item : m_GameState.Items)
    {
        ImGui::Text("%s Lv: %d", item.Name.c_str(), item.Level);
    }

    ImGui::Separator();
    for (auto &resource : m_GameHistory.Resources)
    {
        ImGui::PlotLines(resource.first.c_str(), resource.second.data(), (int)resource.second.size(), 0,
                "History", 10, 500,
                ImVec2(0, 80));
    }

    ImGui::End();
}

void MetaGameSim::OnEvent(AppEvent event, void *eventData)
{
}
