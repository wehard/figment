#pragma once

#include "ScriptEngine.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

class MonoScriptEngine : public ScriptEngine
{
public:
    static Scene *m_Scene;
    MonoScriptEngine() = default;
    ~MonoScriptEngine() = default;
    void Init();
    void Shutdown();
    static Scene *GetSceneContext();

private:
    MonoDomain *m_RootDomain;
    MonoDomain *m_AppDomain;
    MonoAssembly *m_Assembly;
    MonoImage *m_AssemblyImage;

    bool LoadCSharpAssembly(const std::string &assemblyPath);
};
