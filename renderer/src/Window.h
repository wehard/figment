#pragma once

#include <memory>
#include <string>

class Window
{
public:
    virtual ~Window() = default;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    static std::shared_ptr<Window> Create(const std::string &title, const uint32_t width, const uint32_t height);
};
