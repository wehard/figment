#pragma once

#include <cstdint>
#include <unordered_map>

namespace Figment
{
    class UUID
    {
    public:
        UUID();
        explicit UUID(uint64_t uuid);

        explicit operator uint64_t() const
        { return m_UUID; }

    private:
        uint64_t m_UUID;
    };
}

namespace std
{
    template<>
    struct hash<Figment::UUID>
    {
        std::size_t operator()(const Figment::UUID &uuid) const
        {
            return std::hash<uint64_t>()((uint64_t)uuid);
        }
    };
}
