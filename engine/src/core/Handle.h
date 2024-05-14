#pragma once

#include <cstdint>

namespace Figment
{
    template<typename T>
    struct Handle
    {
        explicit Handle(uint32_t index = 0, uint32_t generation = 0) : index(index), generation(generation) { }
        // explicit operator T() { return data; }
        // explicit operator const T() const { return data; }
        //
        // explicit operator T &() { return data; }
        // explicit operator const T &() const { return data; }

        // Equality operators
        // bool operator==(const Handle<T> &other) const
        // {
        //     return index == other.index && generation == other.generation;
        // }

        uint32_t index = 0;
        uint32_t generation = 0;
        T data;
    };
}
