#include "ScriptEngine.h"
#include "CoreCLRScriptEngine.h"

std::unique_ptr<ScriptEngine> ScriptEngine::Create()
{
    return std::make_unique<Figment::CoreCLRScriptEngine>();
}
