#pragma once

#include "Figment.h"

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
        std::string Name;
        uint32_t Amount;
    };

    struct GameItem
    {
        std::string Name;
        uint32_t Level;
    };

    struct GameState
    {
        std::vector<GameResource> Resources;
        std::vector<GameItem> Items;
    };

    struct GameHistory
    {
        std::map<std::string, std::vector<float>> Resources;
    };

    struct Action
    {
        std::string Name;
        std::string Description;
        std::function<void(GameState &)> Function;
    };

private:

    GameState m_GameState;

    std::vector<Action> m_Actions;

    GameHistory m_GameHistory;
};
