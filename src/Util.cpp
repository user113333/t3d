#include "Util.h"

#define JSON_IMPLEMENTATION
#include "Vertex.h"
#include "TextureCPU.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string GetDir(std::string& path) {
    std::size_t lo = path.find_last_of("/\\");

    if (lo == std::string::npos) {
        return "./";
    }

    return path.substr(0, lo);
}

std::string GetNotDir(std::string& path) {
    std::size_t lo = path.find_last_of("/\\");

    if (lo == std::string::npos) {
        return path;
    }

    return path.substr(lo + 1);
}

std::string GetSuffix(std::string& path) {
    std::string name = GetNotDir(path);
    std::size_t lo = name.find_last_of(".");

    if (lo == std::string::npos) {
        return name;
    }

    return name.substr(lo + 1);
}

std::string GetBasename(std::string& path) {
    std::string name = GetNotDir(path);
    std::size_t lo = name.find_last_of(".");

    if (lo == std::string::npos) {
        return "";
    }

    return name.substr(0, lo);
}
