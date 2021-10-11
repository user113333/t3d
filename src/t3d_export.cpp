#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "json.hpp"

#include "Core.h"

struct Vertex {
    glm::vec3 pos;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

struct Model {
    std::vector<Mesh> meshes;
};

using namespace nlohmann;

void LoadModel(std::string const &path, Model& model);
void ProcessNode(aiNode *node, const aiScene *scene, Model& model);
Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

void LoadModel(std::string const &path, Model& model) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ERROR << importer.GetErrorString() << std::endl;
        return;
    }

    ProcessNode(scene->mRootNode, scene, model);
}

void ProcessNode(aiNode *node, const aiScene *scene, Model& model) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model.meshes.push_back(ProcessMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, model);
    }
}

Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        vertices.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return { vertices, indices };
}

void t3d_export(std::string input_file, std::string output_file) {
    Model model;
    json j;
    
    LoadModel(input_file, model);

    if (model.meshes.size() <= 0) {
        return;
    }

    for (int i = 0; i < model.meshes.size(); i++) {
        Mesh* m = &model.meshes[i];

        auto va = j.array();
        json ia(m->indices);

        for (int j = 0; j < m->vertices.size(); j++) {
            Vertex* v = &m->vertices[j];
            va.push_back({{ "x", v->pos.x }, { "y", v->pos.y }, { "z", v->pos.z }});
        }

        j.push_back({{ "v", va }, { "i", ia }});
    }
    
    std::ofstream o(output_file);
    o << std::setw(4) << j << std::endl;
}
