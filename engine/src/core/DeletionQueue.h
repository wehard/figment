#pragma once

#include <functional>
#include <deque>

struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void Push(std::function<void()> &&function)
    {
        deletors.push_back(function);
    }

    void Flush()
    {
        for (auto &function : deletors)
            function();
        deletors.clear();
    }
};
