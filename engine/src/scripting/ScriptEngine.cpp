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
        scriptEngine = std::make_unique<MonoScriptEngine>();
        break;
    }
    case ScriptEngineType::CoreCLR:
        scriptEngine = std::make_unique<Figment::CoreCLRScriptEngine>();
        break;
    default:
        scriptEngine = std::make_unique<MonoScriptEngine>();
        break;
    }
    scriptEngine->m_Scene = scene;
    return scriptEngine;
}

ScriptEngine *ScriptEngine::s_Instance = nullptr;

Scene *ScriptEngine::m_Scene = nullptr;

Scene *ScriptEngine::GetSceneContext()
{
    return m_Scene;
}
