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
                m_FreeList.push(i);
            }
        }
        ~Pool()
        {
            delete[] m_Data;
        }

        uint32_t Capacity() const
        {
            return m_Capacity;
        }

        T *Get()
        {
            uint32_t index = m_FreeList.top();
            m_FreeList.pop();
            return &m_Data[index].data;
        }

        // void Delete(Handle<T> handle)
        // {
        //     auto handlePtr = Get(handle);
        //     if (handlePtr == nullptr)
        //     {
        //         return;
        //     }
        //
        //     m_FreeList.push(handle.index);
        // }
    private:
        uint32_t m_Capacity = 0;
        uint32_t m_Index = 0;
        Handle<T> *m_Data = nullptr;
        std::stack<uint32_t> m_FreeList;

    };
}
