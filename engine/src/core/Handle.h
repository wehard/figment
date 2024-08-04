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

        [[nodiscard]] uint32_t Index() const { return index; }
        [[nodiscard]] uint32_t Generation() const { return generation; }

    private:
        friend class Pool<T>;

        uint32_t index = 0;
        uint32_t generation = 0;
    };
}
