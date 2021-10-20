#pragma once

#include <string>

enum TEXTURE_TYPE {
    TEXTURE_TYPE_BASE,
};

namespace t3d {
    struct Texture {
        unsigned char* data;
        
        int chanel_count;
        int width;
        int height;

        unsigned int texture_type;
    };
}
