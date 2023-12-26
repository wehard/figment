#pragma once

#include <cstdint>
#include <random>
#include "glm/glm.hpp"

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

        static glm::vec4 Color()
        {
            return { Float(0.0, 1.0), Float(0.0, 1.0), Float(0.0, 1.0), 1.0f };
        }
    };
}
