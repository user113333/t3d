#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include "Model.h"
#include "Vertex.h"
#include "Core.h"
#include "Util.h"

using namespace t3d;

aiMatrix4x4 ReadNodeHeirarchy(aiNode* node) {
    aiMatrix4x4 child = node->mTransformation;
    
    aiNode* parent = node;
    while ((parent = parent->mParent) != nullptr) {
        child = parent->mTransformation * child;
    }

    return child;
}

int VTHasH(std::vector<TextureCPU>* textures, unsigned int hash) {
    for (int i = 0; i < textures->size(); i++) {
        if (textures->at(i).hash == hash) {
            return i;
        }
    }

    return -1;
}

static void LoadMaterialTextures(std::vector<TextureCPU>* textures, std::vector<unsigned int>* textures_indices, aiMaterial *mat, aiTextureType assimp_enum, TEXTURE_TYPE type) {
    for (unsigned int i = 0; i < mat->GetTextureCount(assimp_enum); i++) {
        aiString path;
        mat->GetTexture(assimp_enum, i, &path);

        std::string full_path = (working_dir + "/" + path.C_Str());
        unsigned long full_path_hash = HashStr((unsigned char*)full_path.c_str());
        int index = VTHasH(textures, full_path_hash);

        if (index == -1) {
            TextureCPU texture = LoadImageTT(full_path.c_str());
            
            texture.type = type;
            texture.hash = full_path_hash;

            index = textures->size();
            textures->push_back(texture);
        }
        
        textures_indices->push_back(index);
    }
}

static Mesh ProcessMesh(aiMesh *mesh, glm::mat4 mesh_transform, const aiScene *scene, Model* model) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> texture_indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        glm::vec4 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vector.w = 1.0f;

        vector = mesh_transform * vector;
        
        vertex.pos.x = vector.x;
        vertex.pos.y = vector.y;
        vertex.pos.z = vector.z;

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
    
    LoadMaterialTextures(&model->textures, &texture_indices, material, aiTextureType_DIFFUSE, TEXTURE_TYPE_BASE);
 
    return { vertices, indices, texture_indices };
}

static void ProcessNode(aiNode *node, const aiScene *scene, Model* model) {
    aiMatrix4x4 mesh_transform = ReadNodeHeirarchy(node);

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes.push_back(ProcessMesh(mesh, CastMat4(mesh_transform), scene, model));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, model);
    }
}

void LoadModel(std::string const &path, Model* model) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs); // TODO: aiProcess_SortByPType -> lines + points

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ERROR << importer.GetErrorString() << std::endl;
        return;
    }

    ProcessNode(scene->mRootNode, scene, model);
}
