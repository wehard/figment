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
        Pool() = default;
        explicit Pool(uint32_t capacity) : m_Capacity(capacity)
        {
            m_Data = new Handle<T>[capacity];
            for (uint32_t i = 0; i < capacity; i++)
            {
                m_Data[i] = Handle<T>(i, 0);
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

        T *Get(Handle<T> handle)
        {
            auto match = m_Data[handle.index];
            if (match.generation != handle.generation)
            {
                return nullptr;
            }
            return &m_Data[handle.index].data;
        }

        Handle<T> Create()
        {
            if (!m_FreeList.empty() && m_Count < m_Capacity)
            {
                auto index = m_FreeList.top();
                m_FreeList.pop();
                return m_Data[index];
            }
            if (m_Count >= m_Capacity)
            {
                Resize(m_Capacity * 2);
            }
            return m_Data[m_Count++];
        }

        void Delete(Handle<T> handle)
        {
            m_Data[handle.index].generation++;
            m_Count--;
            // m_FreeList.push(handle.index);
        }
    private:
        uint32_t m_Capacity = 0;
        uint32_t m_Count = 0;
        Handle<T> *m_Data = nullptr;
        std::stack<uint32_t> m_FreeList;

        void Resize(uint32_t capacity)
        {
            m_Capacity = capacity;
            auto newData = new Handle<T>[capacity];
            for (uint32_t i = 0; i < m_Count; i++)
            {
                newData[i] = m_Data[i];
            }
            delete[] m_Data;
            m_Data = newData;
        }
    };
}
