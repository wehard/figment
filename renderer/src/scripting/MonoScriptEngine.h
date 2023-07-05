#pragma once

#include "ScriptEngine.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

class MonoScriptEngine : public ScriptEngine
{
public:
    MonoScriptEngine() = default;
    ~MonoScriptEngine() = default;
    void Init();
    void Shutdown();

private:
    MonoDomain *m_RootDomain;
    MonoDomain *m_AppDomain;
    MonoAssembly *m_Assembly;

    MonoAssembly *LoadCSharpAssembly(const std::string &assemblyPath);
};
