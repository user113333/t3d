#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec2 texture_coords;
};

#include "json.hpp"

using namespace nlohmann;

extern void to_json(json& JSON, const Vertex& v);
extern void from_json(const json& JSON, Vertex& v);

#ifdef JSON_IMPLEMENTATION

void to_json(json& JSON, const Vertex& v) {
    JSON = {
        { "pos", {
            { "x", v.pos.x }, { "y", v.pos.y }, { "z", v.pos.z }
        }},
        { "texture_coords", {
            { "x", v.texture_coords.x }, { "y", v.texture_coords.y }
        }}
    };
}

void from_json(const json& JSON, Vertex& v) {
    json j_pos = JSON.at("pos");
    j_pos.at("x").get_to(v.pos.x);
    j_pos.at("y").get_to(v.pos.y);
    j_pos.at("z").get_to(v.pos.z);

    json j_tc = JSON.at("texture_coords");
    j_tc.at("x").get_to(v.texture_coords.x);
    j_tc.at("y").get_to(v.texture_coords.y);
}

#endif
