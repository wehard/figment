#pragma once

#include <cstdint>

namespace Figment
{
    template<typename T>
    struct Handle
    {
    public:
        Handle() = default;
        explicit Handle(T data) : data(data) { }
        T data;

        explicit operator T () { return data; }
        explicit operator const T () const { return data; }

        explicit operator T & () { return data; }

        uint32_t index = 0;
        uint32_t generation = 0;
    };
}
