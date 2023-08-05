#pragma once

#include <memory>

class Scene;

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
    virtual void OnUpdate(float timeStep) = 0;

    static std::unique_ptr<ScriptEngine> Create(ScriptEngineType scriptEngineType, Scene *scene);
    static ScriptEngine *s_Instance;
    static Scene *m_Scene;
    static Scene *GetSceneContext();
};
