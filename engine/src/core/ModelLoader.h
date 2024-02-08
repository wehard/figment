#pragma once

#include <string>
#include <vector>
#include "tiny_gltf.h"

namespace Figment::ModelLoader
{
    struct MeshData
    {
        std::vector<float> Vertices;
        std::vector<uint32_t> Indices;

        uint32_t VertexCount() const
        {
            return Vertices.size();
        }

        uint32_t IndexCount() const
        {
            return Indices.size();
        }
    };

    tinygltf::Model LoadGltf(const std::string &path);
    MeshData LoadObj(const std::string &path);
}
