#pragma once

#include <iostream>
#include <cstdlib>

#define FIGMENT_ASSERT(condition, message)                                     \
    do                                                                         \
    {                                                                          \
        if (!(condition))                                                      \
        {                                                                      \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::exit(EXIT_FAILURE);                                           \
        }                                                                      \
    } while (false)
