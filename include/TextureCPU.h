#pragma once

#include <string>

struct TextureCPU {
    std::string path;
    unsigned int type;
};

#include "json.hpp"

using namespace nlohmann;

extern void to_json(json& JSON, const TextureCPU& t);
extern void from_json(const json& JSON, TextureCPU& t);

#ifdef JSON_IMPLEMENTATION

void to_json(json& JSON, const TextureCPU& t) {
    JSON = { { "path", t.path }, { "type", t.type } };
}

void from_json(const json& JSON, TextureCPU& t) {
    JSON.at("path").get_to(t.path);
    JSON.at("type").get_to(t.type);
}

#endif
