#include "ScriptEngine.h"
#include "CoreCLRScriptEngine.h"
#include "MonoScriptEngine.h"

std::unique_ptr<ScriptEngine> ScriptEngine::Create(ScriptEngineType scriptEngineType, Scene *scene)
{
    std::unique_ptr<ScriptEngine> scriptEngine;

    switch (scriptEngineType)
    {
    case ScriptEngineType::Mono:
    {
        auto mono = std::make_unique<MonoScriptEngine>();
        mono->m_Scene = scene;
        return mono;
        break;
    }
    case ScriptEngineType::CoreCLR:
        return std::make_unique<Figment::CoreCLRScriptEngine>();
        break;
    default:
        return std::make_unique<MonoScriptEngine>();

        break;
    }
}
