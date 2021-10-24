#include "Util.h"

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

t3d::TextureCPU LoadImageTT(const char* path) {
    t3d::TextureCPU tex;

    int x, y, ch_count;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &x, &y, &ch_count, 0);

    tex.chanel_count = ch_count;
    tex.width = x;
    tex.height = y;

    tex.data = data;

    return tex;
}
