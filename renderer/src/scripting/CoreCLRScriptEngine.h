#pragma once

#include "ScriptEngine.h"
#include <hostfxr.h>

namespace Figment
{
    struct HostFXRFunctions
    {
        hostfxr_initialize_for_runtime_config_fn InitFunction;
        hostfxr_get_runtime_delegate_fn GetDelegateFunction;
        hostfxr_close_fn CloseFunction;
    };

    class CoreCLRScriptEngine : public ScriptEngine
    {
    public:
        CoreCLRScriptEngine() = default;
        ~CoreCLRScriptEngine() = default;
        void Init();
        void Shutdown();

    private:
    };

}
