#pragma once

#include <cstdint>

namespace Figment
{
    template<typename T>
    class Pool;

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

        [[nodiscard]] uint32_t Index() const { return index; }
        [[nodiscard]] uint32_t Generation() const { return generation; }

    private:
        friend class Pool<T>;

        uint32_t index = 0;
        uint32_t generation = 0;
        T data;
    };
}
