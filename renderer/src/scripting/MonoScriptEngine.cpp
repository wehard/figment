#include "MonoScriptEngine.h"
#include "FigmentAssert.h"
#include <fstream>

static char *ReadBytes(const std::string &filepath, uint32_t *outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        // Failed to open the file
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0)
    {
        // File is empty
        return nullptr;
    }

    char *buffer = new char[size];
    stream.read((char *)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

static void PrintAssemblyTypes(MonoAssembly *assembly)
{
    MonoImage *image = mono_assembly_get_image(assembly);
    const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char *nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

        printf("%s.%s\n", nameSpace, name);
    }
}

static MonoClass *GetManagedClass(MonoAssembly *assembly, const char *namespaceName, const char *className)
{
    MonoImage *image = mono_assembly_get_image(assembly);
    MonoClass *managedClass = mono_class_from_name(image, namespaceName, className);
    FIGMENT_ASSERT(managedClass != nullptr, "Error getting managed class!");
    return managedClass;
}

void MonoScriptEngine::Init()
{
    mono_set_assemblies_path("mono/lib");

    m_RootDomain = mono_jit_init("FigmentRuntime");
    FIGMENT_ASSERT(m_RootDomain != nullptr, "Failed to init mono root domain!");

    m_AppDomain = mono_domain_create_appdomain("MyAppDomain", nullptr);
    FIGMENT_ASSERT(m_AppDomain != nullptr, "Failed to init mono app domain!");

    mono_domain_set(m_AppDomain, true);

    m_Assembly = LoadCSharpAssembly("scripting-core/FigmentScriptingCore.dll");
    PrintAssemblyTypes(m_Assembly);

    MonoClass *managedClass = GetManagedClass(m_Assembly, "FigmentScriptingCore", "Component");
    MonoObject *managedClassInstance = mono_object_new(m_AppDomain, managedClass);
    FIGMENT_ASSERT(managedClassInstance != nullptr, "Error instantiating managed class!");
    mono_runtime_object_init(managedClassInstance);

    MonoMethod *method = mono_class_get_method_from_name(managedClass, "OnUpdate", 0);
    FIGMENT_ASSERT(method != nullptr, "Error getting method!");

    MonoObject *exception = nullptr;
    mono_runtime_invoke(method, managedClassInstance, nullptr, &exception);
    FIGMENT_ASSERT(exception == nullptr, "Error invoking method!");
}

void MonoScriptEngine::Shutdown()
{
    mono_jit_cleanup(m_RootDomain);
}

MonoAssembly *MonoScriptEngine::LoadCSharpAssembly(const std::string &assemblyPath)
{
    uint32_t fileSize = 0;
    char *fileData = ReadBytes(assemblyPath, &fileSize);

    // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
    MonoImageOpenStatus status;
    MonoImage *image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        const char *errorMessage = mono_image_strerror(status);
        // Log some error message using the errorMessage data
        return nullptr;
    }

    MonoAssembly *assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
    mono_image_close(image);

    // Don't forget to free the file data
    delete[] fileData;

    return assembly;
}
