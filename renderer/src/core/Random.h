#pragma once

#include <cstdint>
#include <random>

namespace Figment
{
    static std::random_device s_Device;
    static std::mt19937_64 s_Engine(s_Device());
    static std::uniform_int_distribution<uint64_t> s_UniformDist;

    class Random
    {
    public:
        static float Float()
        {
            return (float)s_UniformDist(s_Engine) / (float)UINT64_MAX;
        }

        static float Float(float min, float max)
        {
            return min + Float() * (max - min);
        }
    };
}
