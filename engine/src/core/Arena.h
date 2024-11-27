#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>

struct Arena
{
public:
    explicit Arena(uint32_t size): m_Size(size)
    {
        m_Base    = (char*)malloc(size);
        m_Pointer = m_Base;
        bzero(m_Base, size);
    }

    ~Arena() { free(m_Base); }

    template<typename T>
    T* Allocate()
    {
        return (T*)Allocate(sizeof(T));
    }

    template<typename T>
    T* Allocate(uint32_t count)
    {
        return (T*)Allocate(sizeof(T) * count);
    }

    void* Allocate(uint32_t size)
    {
        if (size > m_Size)
            return nullptr;
        if (size == 0 || m_Pointer + size - m_Base > m_Size || m_Pointer == nullptr)
            return nullptr;

        char* ptr = m_Pointer;
        m_Pointer += size;
        return (void*)ptr;
    }

private:
    char* m_Base    = nullptr;
    uint32_t m_Size = 0;
    char* m_Pointer = nullptr;
};
