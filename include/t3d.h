#pragma once

#include <vector>

#include "c_plus_plus_serializer.h"

#define T3D_VERSION 2

enum TEXTURE_TYPE {
    TEXTURE_TYPE_ERROR = -1,
    TEXTURE_TYPE_BASE = 0,
    TEXTURE_TYPE_NORMAL,
    TEXTURE_TYPE_ROUGHNESS,
    TEXTURE_TYPE_METAL,
    TEXTURE_TYPE_SPECULAR,
    TEXTURE_TYPE_HEIGHT,
    TEXTURE_TYPE_OPACITY,
    TEXTURE_TYPE_AMBIENT,
    TEXTURE_TYPE_SELF_ILLUMINATION,
    TEXTURE_TYPE_LAST,
};

namespace t3d {

    struct Model;

    bool Serialize(const char* file_path, Model* model);
    bool Parse(const char* file_path, Model* model);

    struct vec3 {
        float x, y, z;

        friend std::ostream& operator<<(std::ostream& out, Bits<class vec3& > my) {
            out << bits(my.t.x) << bits(my.t.y) << bits(my.t.z);
            return (out);
        }

        friend std::istream& operator>>(std::istream& in, Bits<class vec3&> my) {
            in >> bits(my.t.x) >> bits(my.t.y) >> bits(my.t.z);
            return (in);
        }
    };

    struct vec2 {
        float x, y;

        friend std::ostream& operator<<(std::ostream& out, Bits<class vec2& > my) {
            out << bits(my.t.x) << bits(my.t.y);
            return (out);
        }

        friend std::istream& operator>>(std::istream& in, Bits<class vec2&> my) {
            in >> bits(my.t.x) >> bits(my.t.y);
            return (in);
        }
    };

    struct Vertex {
        vec3 pos;
        vec3 normal;
        vec2 texture_uv;

        friend std::ostream& operator<<(std::ostream& out, Bits<class Vertex& > my) {
            out << bits(my.t.pos) << bits(my.t.normal) << bits(my.t.texture_uv);
            return (out);
        }

        friend std::istream& operator>>(std::istream& in, Bits<class Vertex&> my) {
            in >> bits(my.t.pos) >> bits(my.t.normal) >> bits(my.t.texture_uv);
            return (in);
        }
    };

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        int textures[TEXTURE_TYPE_LAST];

        friend std::ostream& operator<<(std::ostream& out, Bits<class Mesh& > my) {
            out << bits(my.t.vertices) << bits(my.t.indices) << bits(my.t.textures);
            return (out);
        }

        friend std::istream& operator>>(std::istream& in, Bits<class Mesh&> my) {
            in >> bits(my.t.vertices) >> bits(my.t.indices) >> bits(my.t.textures);
            return (in);
        }
    };

    struct Texture {
        int chanel_count;
        int width;
        int height;

        std::string name;
        unsigned char* data;

        friend std::ostream& operator<<(std::ostream& out, Bits<class Texture& > my) {
            out << bits(my.t.chanel_count) << bits(my.t.width) << bits(my.t.height) << bits(my.t.name) << bits(my.t.data);
            return (out);
        }

        friend std::istream& operator>>(std::istream& in, Bits<class Texture&> my) {
            in >> bits(my.t.chanel_count) >> bits(my.t.width) >> bits(my.t.height) >> bits(my.t.name) >> bits(my.t.data);
            return (in);
        }
    };

    struct Model {
        int32_t version;
        std::vector<Mesh> meshes;
        std::vector<Texture> textures;

        friend std::ostream& operator<<(std::ostream& out, Bits<class Model&> my) {
            out << bits(my.t.version) << bits(my.t.meshes) << bits(my.t.textures);
            return (out);
        }

        friend std::istream& operator>>(std::istream& in, Bits<class Model&> my) {
            in >> bits(my.t.version) >> bits(my.t.meshes) >> bits(my.t.textures);
            return (in);
        }
    };

}

#ifdef T3D_IMPLEMENTATION

#include <fstream>
#include <cstdint>

namespace t3d {

    bool Serialize(const char* file_path, Model* model) {
        std::ofstream file;
        file.open(file_path, std::ios::trunc | std::ios::binary);

        if (!file.is_open()) {
            return true;
        }

        file << bits(*model);

        file.close();
        return false;
    }

    bool Parse(const char* file_path, t3d::Model* model) {
        std::ifstream file;
        file.open(file_path, std::ios::in | std::ios::binary);

        if (!file.is_open()) {
            return true;
        }

        file >> bits(model->version);

        if (model->version != T3D_VERSION) {
            return true;
        }

        file >> bits(model->meshes);
        file >> bits(model->textures);

        file.close();
        return false;
    }

}

#endif
