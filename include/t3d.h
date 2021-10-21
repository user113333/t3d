#pragma once

#include "Model.h"

#define T3D_VERSION 0

bool t3d_serialize(const char* file_path, t3d::Model& model);
bool t3d_parse(const char* file_path, t3d::Model& model);

#ifdef T3D_IMPLEMENTATION

#include <fstream>
#include <cstdint>

struct file_desc {
    uint8_t version;
    uint8_t meshes_count;
};

struct file_mesh {
    uint16_t vertex_count;
    uint16_t index_count;
};

bool t3d_serialize(const char* file_path, t3d::Model& model) {
    std::ofstream file;
    file.open(file_path, std::ios::trunc | std::ios::binary);

    if (!file.is_open()) {
        return true;
    }

    file_desc desc;
    desc.version = T3D_VERSION;
    desc.meshes_count = model.meshes.size();
    file.write((char*)&desc, sizeof(file_desc));

    for (int i = 0; i < desc.meshes_count; i++) {
        t3d::Mesh* m = &model.meshes[i];
        
        file_mesh mesh;
        mesh.vertex_count = m->vertices.size();
        mesh.index_count = m->indices.size();

        file.write((char*)&mesh, sizeof(file_mesh));
        int size_v = mesh.vertex_count * sizeof(t3d::Vertex);
        file.write((char*)&m->vertices[0], size_v);
        int size_i = mesh.index_count * sizeof(unsigned int);
        file.write((char*)&m->indices[0], size_i);

        // TODO: texture
    }

    file.close();
    return false;
}

bool t3d_parse(const char* file_path, t3d::Model& model) {
    std::ifstream file;
    file.open(file_path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        return true;
    }

    file_desc desc;
    file.read((char*)&desc, sizeof(file_desc));

    for (int i = 0; i < desc.meshes_count; i++) {
        t3d::Mesh m;
        
        file_mesh mesh = { 0, 0 };
        file.read((char*)&mesh, sizeof(file_mesh));

        int size_v = mesh.vertex_count * sizeof(t3d::Vertex);
        m.vertices.resize(mesh.vertex_count);
        int size = m.vertices.end() - m.vertices.begin();
        file.read((char*)(&m.vertices[0]), size_v);

        int size_i = mesh.index_count * sizeof(unsigned int);
        m.indices.resize(mesh.index_count);
        file.read((char*)(&m.indices[0]), size_i);

        model.meshes.push_back(m);
    }

    file.close();
    return false;
}

#endif
