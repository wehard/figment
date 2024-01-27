#include "ModelLoader.h"
#include "Log.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

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
}
