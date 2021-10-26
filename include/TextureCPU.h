#pragma once

#include <string>

enum TEXTURE_TYPE {
    TEXTURE_TYPE_NONE = 0,
    TEXTURE_TYPE_BASE,
};

namespace t3d {
    struct TextureCPU {
        int chanel_count;
        int width;
        int height;
        TEXTURE_TYPE type;
        unsigned long hash;

        unsigned char* data;
    };
}
