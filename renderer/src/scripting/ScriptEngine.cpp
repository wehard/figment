#include "ScriptEngine.h"
#include "CoreCLRScriptEngine.h"
#include "MonoScriptEngine.h"

std::unique_ptr<ScriptEngine> ScriptEngine::Create(ScriptEngineType scriptEngineType)
{
    switch (scriptEngineType)
    {
    case ScriptEngineType::Mono:
        return std::make_unique<MonoScriptEngine>();
        break;
    case ScriptEngineType::CoreCLR:
        return std::make_unique<Figment::CoreCLRScriptEngine>();
    default:
        return std::make_unique<MonoScriptEngine>();

        break;
    }
}
