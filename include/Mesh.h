#pragma once

#include <vector>

#include "Vertex.h"
#include "Texture.h"

namespace t3d {
    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
    };
}
