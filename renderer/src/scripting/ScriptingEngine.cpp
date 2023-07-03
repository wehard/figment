#include "ScriptingEngine.h"
#include <iostream>

#include <hostfxr.h>
#include <dlfcn.h>

namespace Figment
{
    static void *loadLibrary(char_t *path)
    {
        return dlopen(path, RTLD_NOW);
    }

    static void *getExport(void *hModule, const char *name)
    {
        return dlsym(hModule, name);
    }

    void ScriptingEngine::Init()
    {
        char_t buffer[512];
        size_t bufferSize = sizeof(buffer) / sizeof(char_t);
        int result = get_hostfxr_path(buffer, &bufferSize, nullptr);
        if (result != 0)
        {
            std::cout << "Failed to get hostxfr path!" << std::endl;
            return;
        }

        void *lib = loadLibrary(buffer);

        hostfxr_initialize_for_runtime_config_fn initFunction = (hostfxr_initialize_for_runtime_config_fn)getExport(lib, "hostfxr_initialize_for_runtime_config");
        hostfxr_get_runtime_delegate_fn getDelegateFunction = (hostfxr_get_runtime_delegate_fn)getExport(lib, "hostfxr_get_runtime_delegate");
        hostfxr_close_fn closeFunction = (hostfxr_close_fn)getExport(lib, "hostfxr_close");

        const char_t *runtimeConfigPath = "scripting-core/FigmentScriptingCore.runtimeconfig.json";

        hostfxr_handle ctx = nullptr;
        int res = initFunction(runtimeConfigPath, nullptr, &ctx);
        if (res != 0 || ctx == nullptr)
        {
            std::cout << "Failed to init CoreCLR runtime!" << std::endl;
        }

        load_assembly_and_get_function_pointer_fn loadAssemblyGetFunctionPointer = nullptr;
        res = getDelegateFunction(ctx, hdt_load_assembly_and_get_function_pointer, (void **)&loadAssemblyGetFunctionPointer);
        if (res != 0 || loadAssemblyGetFunctionPointer == nullptr)
        {
            // Getting delegate failed
            closeFunction(ctx);
            return;
        }

        // At this point you can call 'load_assembly_and_get_function_pointer' to load a .NET Core assembly and get a function pointer to a static method
        // This allows you to call a .NET Core method from C++ code
        const char_t *assemblyPath = "scripting-core/FigmentScriptingCore.dll";
        const char_t *typeName = "FigmentScriptingCore.Component";
        const char_t *methodName = "FigmentScriptingCore.Component.OnUpdate";
        const char_t *delegateTypeName = "System.Action";

        component_entry_point_fn delegateFnPtr = nullptr;

        res = loadAssemblyGetFunctionPointer(assemblyPath, typeName, methodName, delegateTypeName, nullptr, (void **)&delegateFnPtr);

        if (res != 0)
        {
            std::cout << "Failed to get function pointer!" << std::endl;
        }

        // Close the context when you are done
        closeFunction(ctx);
    }

    void ScriptingEngine::Shutdown()
    {
    }
}
