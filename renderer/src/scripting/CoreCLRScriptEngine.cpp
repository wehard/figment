#include "CoreCLRScriptEngine.h"
#include "FigmentAssert.h"

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
        FIGMENT_ASSERT(sym != nullptr, "getExport failed!");
        return sym;
    }

    static void *loadHostFXR()
    {
        char_t buffer[512];
        size_t bufferSize = sizeof(buffer) / sizeof(char_t);
        int res = get_hostfxr_path(buffer, &bufferSize, nullptr);
        FIGMENT_ASSERT(res == 0, "Failed to get hostxfr path!");

        void *lib = loadLibrary(buffer);
        FIGMENT_ASSERT(lib != nullptr, "Failed to load hostxfr!");

        return lib;
    }

    static HostFXRFunctions GetHostFxrFunctions(void *lib)
    {
        HostFXRFunctions functions;
        functions.InitFunction = (hostfxr_initialize_for_runtime_config_fn)getExport(lib, "hostfxr_initialize_for_runtime_config");
        functions.GetDelegateFunction = (hostfxr_get_runtime_delegate_fn)getExport(lib, "hostfxr_get_runtime_delegate");
        functions.CloseFunction = (hostfxr_close_fn)getExport(lib, "hostfxr_close");
        FIGMENT_ASSERT(functions.InitFunction != nullptr && functions.GetDelegateFunction != nullptr && functions.CloseFunction != nullptr, "Failed to get exports!");
        return functions;
    }

    // static void LoadAssemblyAndFunctionsTest(HostFXRFunctions hostFunctions, hostfxr_handle ctx)
    // {
    //     // Load assembly
    //     void *loadAssembly = nullptr;
    //     int res = hostFunctions.GetDelegateFunction(ctx, hdt_load_in_memory_assembly, (void **)&loadAssembly);
    //     ASSERT(res == 0, "Failed to get delegate for hdt_load_in_memory_assembly!");

    //     get_function_pointer_fn getFunctionPointer = nullptr;
    //     res = hostFunctions.GetDelegateFunction(ctx, hdt_get_function_pointer, (void **)getFunctionPointer);
    //     ASSERT(res == 0, "Failed to get delegate hdt_get_function_pointer!");

    //     custom_entry_point_fn OnUpdate = nullptr;
    //     res = getFunctionPointer(typeName, methodName, delegateTypeName, nullptr, nullptr, (void **)&OnUpdate);
    //     ASSERT(res == 0, "Failed to get function pointer!");
    // }

    void CoreCLRScriptEngine::Init()
    {

        void *lib = loadHostFXR();

        HostFXRFunctions hostFunctions = GetHostFxrFunctions(lib);
        const char_t *runtimeConfigPath = "script-core/ScriptCore.runtimeconfig.json";

        hostfxr_handle ctx = nullptr;
        int res = hostFunctions.InitFunction(runtimeConfigPath, nullptr, &ctx);
        FIGMENT_ASSERT(res == 0, "Initialize CoreCLR runtime failed!");
        FIGMENT_ASSERT(ctx != nullptr, "Failed to get CoreCRL handle!");

        load_assembly_and_get_function_pointer_fn loadAssemblyGetFunctionPointer = nullptr;
        res = hostFunctions.GetDelegateFunction(ctx, hdt_load_assembly_and_get_function_pointer, (void **)&loadAssemblyGetFunctionPointer);
        FIGMENT_ASSERT(res == 0, "Failed to get delegate!");

        const char_t *assemblyPath = "script-core/ScriptCore.dll";
        const char_t *typeName = "Figment.CoreCLR, Figment";
        const char_t *methodName = "Test";
        const char_t *delegateTypeName = "System.Action";

        typedef void(CORECLR_DELEGATE_CALLTYPE * custom_entry_point_fn)();
        custom_entry_point_fn delegateFnPtr = nullptr;

        res = loadAssemblyGetFunctionPointer(assemblyPath, typeName, methodName, delegateTypeName, nullptr, (void **)&delegateFnPtr);
        FIGMENT_ASSERT(res == 0, "Failed to get function pointer!");

        delegateFnPtr();

        // Close the context when you are done
        hostFunctions.CloseFunction(ctx);

        unloadLibrary(lib);
    }

    void CoreCLRScriptEngine::Shutdown()
    {
    }
}
