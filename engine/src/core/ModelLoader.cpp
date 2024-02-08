#include "ModelLoader.h"
#include "Log.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace Figment::ModelLoader
{
    tinygltf::Model LoadGltf(const std::string &path)
    {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);

        if (!warn.empty())
        {
            FIG_LOG_WARN("%s", warn.c_str());
        }

        if (!err.empty())
        {
            FIG_LOG_ERROR("%s", err.c_str());
        }

        if (!ret)
        {
            FIG_LOG_ERROR("Failed to parse glTF");
        }
        return model;
    }

    MeshData LoadObj(const std::string &path)
    {
        tinyobj::ObjReader reader;
        tinyobj::ObjReaderConfig readerConfig;
        readerConfig.triangulate = false;

        if (!reader.ParseFromFile(path, readerConfig))
        {
            FIG_LOG_ERROR("Failed to parse .obj file %s", path.c_str());
            return {};
        }
        MeshData meshData;
        auto &attrib = reader.GetAttrib();
        auto &shapes = reader.GetShapes();
        meshData.Vertices = attrib.GetVertices();
        auto shape = shapes[0];
        for (const auto &index : shape.mesh.indices)
        {
            meshData.Indices.push_back(index.vertex_index);
        }
        FIG_LOG_INFO("Loaded .obj file %s, VertexCount = %d, IndexCount = %d", path.c_str(), meshData.VertexCount(),
                meshData.IndexCount());
        return meshData;
    }
}
