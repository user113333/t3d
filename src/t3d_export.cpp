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
#include <stb_image.h>

#include "json.hpp"
#include "base64.h"

#include "Core.h"
#include "Util.h"

#define T3D_IMPLEMENTATION
#include "Model.h"
#include "t3d.h"

using namespace t3d;
using namespace nlohmann;

static std::string directory;

void LoadModel(std::string const &path, Model& model);
void ProcessNode(aiNode *node, const aiScene *scene, Model& model);
Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type);
void LoadImage(std::string path, Texture& tex);

void LoadModel(std::string const &path, Model& model) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_SortByPType);

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
    std::vector<Texture> textures;

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

        if (face.mNumIndices != 3) {
            ERROR << face.mNumIndices << std::endl;
        }

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


    return { vertices, indices, textures };
}

std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type) {
    std::vector<Texture> textures;
    
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        Texture texture;
        texture.texture_type = TEXTURE_TYPE_BASE;
        LoadImage(str.C_Str(), texture);
        
        textures.push_back(texture);
    }

    return textures;
}

void LoadImage(std::string path, Texture& tex) {
    int x, y, ch_count;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load((directory + "/" + path).c_str(), &x, &y, &ch_count, 0);

    tex.chanel_count = ch_count;
    tex.width = x;
    tex.height = y;

    tex.data = data;
}

void t3d_export(std::string input_file, std::string output_file) {
    directory = GetDir(input_file);
    Model model;
    json j;
    
    LoadModel(input_file, model);

    if (model.meshes.size() <= 0) {
        ERROR << "0 meshes loaded";
        return;
    }

    t3d_serialize(output_file.c_str(), model);
}
