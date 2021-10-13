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

#include "Vertex.h"
#include "TextureCPU.h"

using namespace nlohmann;

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureCPU> textures;
};

struct Model {
    std::vector<Mesh> meshes;
};

void LoadModel(std::string const &path, Model& model);
void ProcessNode(aiNode *node, const aiScene *scene, Model& model);
Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
std::vector<TextureCPU> LoadMaterialTextures(aiMaterial *mat, aiTextureType type);

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
    std::vector<TextureCPU> textures;

    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_coords = vec;
        } else {
            vertex.texture_coords = glm::vec2(0.0f, 0.0f);  
        }

        vertices.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. diffuse maps
    std::vector<TextureCPU> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<TextureCPU> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<TextureCPU> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<TextureCPU> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


    return { vertices, indices, textures };
}

std::vector<TextureCPU> LoadMaterialTextures(aiMaterial *mat, aiTextureType type) {
    std::vector<TextureCPU> textures;
    
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        TextureCPU texture;
        texture.path = str.C_Str();
        texture.type = type;
        
        textures.push_back(texture);
    }

    return textures;
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
        json ta = j.array();

        for (int j = 0; j < m->vertices.size(); j++) {
            va.push_back(m->vertices[j]);
        }

        for (int j = 0; j < m->textures.size(); j++) {
            ta.push_back(m->textures[j]);
        }

        j.push_back({
            { "textures", ta },
            { "v", va },
            { "i", ia }
        });
    }
    
    std::ofstream o(output_file);
    o << std::setw(4) << j << std::endl;
}
