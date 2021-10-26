#pragma once

#include <unordered_map>
#include <string>

#include "Mesh.h"

namespace t3d {
    struct Model {
        std::vector<Mesh> meshes;
        std::vector<TextureCPU> textures;
    };
}
