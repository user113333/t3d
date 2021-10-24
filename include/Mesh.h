#pragma once

#include <vector>

#include "Vertex.h"
#include "TextureCPU.h"

namespace t3d {
    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<TextureCPU> textures;
    };
}
