#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model.h"
#include "Vertex.h"
#include "Core.h"
#include "Util.h"

using namespace t3d;

static void LoadMaterialTextures(std::vector<TextureCPU>* textures, aiMaterial *mat, aiTextureType assimp_enum, unsigned int texture_enum) {
    for (unsigned int i = 0; i < mat->GetTextureCount(assimp_enum); i++) {
        aiString path;
        mat->GetTexture(assimp_enum, i, &path);

        TextureCPU texture = LoadImageTT((working_dir + "/" + path.C_Str()).c_str());
        texture.texture_type = texture_enum;
        
        textures->push_back(texture);
    }
}

static Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene) {
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

        if (face.mNumIndices != 3) {
            ERROR << "non triangle primitive detected: "  << face.mNumIndices << std::endl;
        }

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    LoadMaterialTextures(&textures, material, aiTextureType_DIFFUSE, TEXTURE_TYPE_BASE);

    return { vertices, indices, textures };
}

static void ProcessNode(aiNode *node, const aiScene *scene, Model& model) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model.meshes.push_back(ProcessMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, model);
    }
}

void LoadModel(std::string const &path, Model& model) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_FlipUVs); // TODO: aiProcess_SortByPType -> lines + points

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ERROR << importer.GetErrorString() << std::endl;
        return;
    }

    ProcessNode(scene->mRootNode, scene, model);
}
