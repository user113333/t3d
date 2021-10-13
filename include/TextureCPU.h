#pragma once

#include <string>

struct TextureCPU {
    std::string path;
    unsigned int type;
};

#ifdef JSON_IMPLEMENTATION

#include "json.hpp"

using namespace nlohmann;

inline void to_json(json& JSON, const TextureCPU& t) {
    JSON = { { "path", t.path }, { "type", t.type } };
}

inline void from_json(const json& JSON, TextureCPU& t) {
    JSON.at("path").get_to(t.path);
    JSON.at("type").get_to(t.type);
}

#endif
