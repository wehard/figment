#pragma once

#include <nethost.h>
#include <hostfxr.h>
#include <coreclr_delegates.h>

namespace Figment
{
    class ScriptingEngine
    {
    public:
        ScriptingEngine() = delete;
        ~ScriptingEngine() = delete;
        static void Init();
        static void Shutdown();

    private:
    };

}
