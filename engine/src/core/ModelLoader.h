#pragma once

#include <string>
#include "tiny_gltf.h"

namespace Figment::ModelLoader
{
    tinygltf::Model LoadGltf(const std::string &path);
}
