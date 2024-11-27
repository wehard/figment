#pragma once

#include <cstdint>
#include <unordered_map>

namespace figment
{
class UUID
{
public:
    UUID();
    explicit UUID(uint64_t uuid);

    explicit operator uint64_t() const { return m_UUID; }

private:
    uint64_t m_UUID;
};
} // namespace figment

namespace std
{
template<>
struct hash<figment::UUID>
{
    std::size_t operator()(const figment::UUID& uuid) const
    {
        return std::hash<uint64_t>()((uint64_t)uuid);
    }
};
} // namespace std
