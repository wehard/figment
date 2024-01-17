#include "ModelLoader.h"
#include "Log.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"


namespace Figment::ModelLoader
{
    static void ProcessMesh(cgltf_mesh *mesh)
    {
        FIG_LOG_INFO("Processing GLTF mesh: %s", mesh->name);
        for (int i = 0; i < mesh->primitives_count; ++i)
        {
            auto *primitive = &mesh->primitives[i];
            for (int j = 0; j < primitive->attributes_count; ++j)
            {
                auto attribute = &primitive->attributes[j];
                FIG_LOG_INFO("Attribute: %s", attribute->name);
                if (attribute->name == std::string("POSITION"))
                {
                    FIG_LOG_INFO("Position count: %d", attribute->data->count);
                    cgltf_float positions[attribute->data->count];
                    cgltf_accessor_unpack_floats(attribute->data, positions, attribute->data->count);
                    FIG_LOG_INFO("Position: %f %f %f", positions[0], positions[1], positions[2]);
                }
            }
        }
    }

    void LoadGltf(const std::string &path)
    {
        cgltf_options options = {};
        cgltf_data* data = nullptr;
        cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);

        FIG_LOG_INFO("Loading gltf file: %s, Mesh count: %d", path.c_str(), data->meshes_count);

        for (int i = 0; i < data->meshes_count; ++i)
        {
            auto *mesh = &data->meshes[i];
            ProcessMesh(mesh);
        }
        
        if (result != cgltf_result_success)
        {
            FIG_LOG_ERROR("Failed to load gltf file: %s", path.c_str());
        }
        cgltf_free(data);
    }
}
