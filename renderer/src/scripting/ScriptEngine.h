#pragma once

#include <memory>

enum class ScriptEngineType
{
    Mono,
    CoreCLR
};

class ScriptEngine
{
public:
    virtual ~ScriptEngine() = default;
    virtual void Init() = 0;
    virtual void Shutdown() = 0;

    static std::unique_ptr<ScriptEngine> Create(ScriptEngineType scriptEngineType);
};
