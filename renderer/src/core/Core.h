#pragma once

#if defined(__APPLE__)
#define FIGMENT_MACOS
#elif defined(__EMSCRIPTEN__)
#define FIGMENT_WEB
#endif
#include <memory>

namespace Figment {

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr UniquePtr<T> CreateUniquePtr(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr SharedPtr<T> CreateSharedPtr(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

}
