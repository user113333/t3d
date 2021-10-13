#pragma once

#include <string>

struct TextureCPU {
    std::string data_base64;
    unsigned int type;

    unsigned int gl_format;
    int width;
    int height;
};

#include "json.hpp"

using namespace nlohmann;

extern void to_json(json& JSON, const TextureCPU& t);
extern void from_json(const json& JSON, TextureCPU& t);

#ifdef JSON_IMPLEMENTATION

void to_json(json& JSON, const TextureCPU& t) {
    JSON = {
        { "type", t.type },
        { "gl_format", t.gl_format },
        { "width", t.width },
        { "height", t.height },
        { "data", t.data_base64 }
    };
}

void from_json(const json& JSON, TextureCPU& t) {
    JSON.at("type").get_to(t.type);
    JSON.at("gl_format").get_to(t.gl_format);
    JSON.at("width").get_to(t.width);
    JSON.at("height").get_to(t.height);
    JSON.at("data").get_to(t.data_base64);
}

#endif
