#pragma once

#include <hostfxr.h>

namespace Figment
{
    struct HostFXRFunctions
    {
        hostfxr_initialize_for_runtime_config_fn InitFunction;
        hostfxr_get_runtime_delegate_fn GetDelegateFunction;
        hostfxr_close_fn CloseFunction;
    };

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
