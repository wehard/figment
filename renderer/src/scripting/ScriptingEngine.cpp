#include "ScriptingEngine.h"
#include "../Assert.h"

#include <iostream>

#include <nethost.h>
#include <hostfxr.h>
#include <dlfcn.h>
#include <coreclr_delegates.h>

namespace Figment
{
    static void *loadLibrary(char_t *path)
    {
        return dlopen(path, RTLD_NOW);
    }

    static void unloadLibrary(void *lib)
    {
        dlclose(lib);
    }

    static void *getExport(void *hModule, const char *name)
    {
        void *sym;
        sym = dlsym(hModule, name);
        ASSERT(sym != nullptr, "getExport failed!");
        return sym;
    }

    static void *loadHostFXR()
    {
        char_t buffer[512];
        size_t bufferSize = sizeof(buffer) / sizeof(char_t);
        int res = get_hostfxr_path(buffer, &bufferSize, nullptr);
        ASSERT(res == 0, "Failed to get hostxfr path!");

        void *lib = loadLibrary(buffer);
        ASSERT(lib != nullptr, "Failed to load hostxfr!");

        return lib;
    }

    static HostFXRFunctions GetHostFxrFunctions(void *lib)
    {
        HostFXRFunctions functions;
        functions.InitFunction = (hostfxr_initialize_for_runtime_config_fn)getExport(lib, "hostfxr_initialize_for_runtime_config");
        functions.GetDelegateFunction = (hostfxr_get_runtime_delegate_fn)getExport(lib, "hostfxr_get_runtime_delegate");
        functions.CloseFunction = (hostfxr_close_fn)getExport(lib, "hostfxr_close");
        ASSERT(functions.InitFunction != nullptr && functions.GetDelegateFunction != nullptr && functions.CloseFunction != nullptr, "Failed to get exports!");
        return functions;
    }

    void ScriptingEngine::Init()
    {

        void *lib = loadHostFXR();

        HostFXRFunctions hostFunctions = GetHostFxrFunctions(lib);
        const char_t *runtimeConfigPath = "scripting-core/FigmentScriptingCore.runtimeconfig.json";

        hostfxr_handle ctx = nullptr;
        int res = hostFunctions.InitFunction(runtimeConfigPath, nullptr, &ctx);
        ASSERT(res == 0, "Initialize CoreCLR runtime failed!");
        ASSERT(ctx != nullptr, "Failed to get CoreCRL handle!");

        load_assembly_and_get_function_pointer_fn loadAssemblyGetFunctionPointer = nullptr;
        res = hostFunctions.GetDelegateFunction(ctx, hdt_load_assembly_and_get_function_pointer, (void **)&loadAssemblyGetFunctionPointer);
        ASSERT(res == 0, "Failed to get delegate!");

        const char_t *assemblyPath = "scripting-core/FigmentScriptingCore.dll";
        const char_t *typeName = "FigmentScriptingCore.Component, FigmentScriptingCore";
        const char_t *methodName = "OnUpdate";
        const char_t *delegateTypeName = "System.Action";

        typedef void(CORECLR_DELEGATE_CALLTYPE * custom_entry_point_fn)();
        custom_entry_point_fn delegateFnPtr = nullptr;

        res = loadAssemblyGetFunctionPointer(assemblyPath, typeName, methodName, delegateTypeName, nullptr, (void **)&delegateFnPtr);
        ASSERT(res == 0, "Failed to get function pointer!");

        delegateFnPtr();

        // Close the context when you are done
        hostFunctions.CloseFunction(ctx);

        unloadLibrary(lib);
    }

    void ScriptingEngine::Shutdown()
    {
    }
}
