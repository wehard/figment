#pragma once

#include <cstdint>
#include <stack>
#include "Handle.h"

namespace Figment
{

    template<typename T>
    class Pool
    {
    public:
        explicit Pool() : Pool(DefaultCapacity) { };
        explicit Pool(uint32_t capacity) : m_Capacity(capacity)
        {
            m_Data = new Slot[capacity];
            for (uint32_t i = 0; i < capacity; i++)
            {
                m_Data[i] = Slot { .generation = 0, .data = T() };
            }
        }

        ~Pool()
        {
            delete[] m_Data;
        }

        [[nodiscard]] uint32_t Capacity() const
        {
            return m_Capacity;
        }

        [[nodiscard]] uint32_t Count() const
        {
            return m_Count;
        }

        T *Get(Handle<T> handle)
        {
            auto match = m_Data[handle.index];
            if (match.generation != handle.generation)
            {
                return nullptr;
            }
            return &m_Data[handle.index].data;
        }

        Handle<T> Create(const T data)
        {
            if (!m_FreeList.empty() && m_Count < m_Capacity)
            {
                auto index = m_FreeList.top();
                m_FreeList.pop();
                m_Data[index] = Slot { .generation = m_Data[index].generation + 1, .data = data };
                return Handle<T>(index, m_Data[index].generation);
            }
            if (m_Count >= m_Capacity)
            {
                Resize(m_Capacity * 2);
            }
            auto count = m_Count;
            m_Data[count] = Slot { .generation = m_Data[count].generation, .data = data };
            m_Count++;
            return Handle<T>(count, m_Data[count].generation);
        }

        void Delete(Handle<T> handle)
        {
            m_Data[handle.index].generation++;
            m_Count--;
            // m_FreeList.push(handle.index);
        }

        constexpr static uint32_t DefaultCapacity = 10;
    private:
        struct Slot
        {
            uint32_t generation;
            T data;
        };

        uint32_t m_Capacity = DefaultCapacity;
        uint32_t m_Count = 0;
        Slot *m_Data = nullptr;
        std::stack<uint32_t> m_FreeList;

        void Resize(uint32_t capacity)
        {
            m_Capacity = capacity;
            auto newData = new Slot[capacity];
            for (uint32_t i = 0; i < m_Count; i++)
            {
                newData[i] = m_Data[i];
            }
            delete[] m_Data;
            m_Data = newData;
        }
    };
}
