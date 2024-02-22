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
        constexpr static const float PI = 3.14159265358979323846f;

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

        static glm::vec2 InsideUnitCircle()
        {
            auto radius = Float() + Float();
            if (radius > 1.0f)
            {
                radius = 2.0f - radius;
            }
            float theta = Float(0.0f, 2.0f * PI);
            return { radius * cos(theta), radius * sin(theta) };
        }

        static glm::vec2 InRing(float innerRadius, float outerRadius)
        {
            float theta = Float(0.0f, 2.0f * PI);
            float w = Float(0.0f, 1.0f);
            float radius = sqrt((1.0f - w) * innerRadius * innerRadius + w * outerRadius * outerRadius);
            return { radius * cos(theta), radius * sin(theta) };
        }

        static glm::vec3 InsideUnitSphere()
        {
            auto radius = Float() + Float();
            if (radius > 1.0f)
            {
                radius = 2.0f - radius;
            }
            float theta = Float(0.0f, 2.0f * PI);
            float phi = Float(0.0f, PI);
            return { radius * sin(phi) * cos(theta), radius * sin(phi) * sin(theta), radius * cos(phi) };
        }
    };
}
