#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace t3d {
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 texture_coords;
    };
}
