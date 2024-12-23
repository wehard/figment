#pragma once

#include "Handle.h"
#include <cstdint>
#include <cstring>
#include <functional>
#include <stack>

namespace figment
{
template<typename T>
class Pool
{
public:
    Pool() = delete;
    explicit Pool(uint32_t capacity, std::function<void(T* item)>&& onDispose = nullptr):
        m_Capacity(capacity), m_OnDispose(onDispose)
    {
        m_Data        = (T*)malloc(sizeof(T) * capacity);
        m_Generations = new uint32_t[capacity];
    }

    ~Pool()
    {
        if (m_OnDispose)
        {
            for (uint32_t i = 0; i < m_Count; i++)
            {
                m_OnDispose(&m_Data[i]);
            }
        }
        free(m_Data);
    }

    [[nodiscard]] uint32_t Capacity() const { return m_Capacity; }

    [[nodiscard]] uint32_t Count() const { return m_Count; }

    T* Get(Handle<T> handle)
    {
        auto currentGeneration = m_Generations[handle.index];
        if (currentGeneration != handle.generation)
        {
            return nullptr;
        }
        return &m_Data[handle.index];
    }

    template<typename... Args>
    Handle<T> Create(Args&&... args)
    {
        auto data = T(std::forward<Args>(args)...);

        memcpy(&m_Data[m_Count], &data, sizeof(T));
        auto handle = Handle<T>(m_Count, m_Generations[m_Count]);
        m_Count++;
        return handle;
    }

    void Delete(Handle<T> handle)
    {
        m_Data[handle.index].generation++;
        m_Count--;
        m_FreeList.push(handle.index);
    }

    constexpr static uint32_t DefaultCapacity = 10;

private:
    uint32_t m_Capacity = DefaultCapacity;
    uint32_t m_Count    = 0;
    T* m_Data           = nullptr;
    uint32_t* m_Generations;
    std::stack<uint32_t> m_FreeList;

    const std::function<void(T* item)>& m_OnDispose = nullptr;

    void Resize(uint32_t capacity) {}
};
} // namespace figment
