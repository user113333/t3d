#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include <string>

#include "Util.h"
#include "t3d.h"
#include "Core.h"

using namespace t3d;

aiMatrix4x4 ReadNodeHeirarchy(aiNode* node) {
    aiMatrix4x4 child = node->mTransformation;
    
    aiNode* parent = node;
    while ((parent = parent->mParent) != nullptr) {
        child = parent->mTransformation * child;
    }

    return child;
}

int VTHasN(std::vector<Texture>* textures, const char* name) {
    for (int i = 0; i < textures->size(); i++) {
        if (textures->at(i).name.compare(name) == 0) {
            return i;
        }
    }

    return -1;
}

static int LoadMaterialTextures(aiMaterial *mat, std::vector<Texture>* textures, aiTextureType assimp_enum) {
    int index = -1;

    for (unsigned int i = 0; i < mat->GetTextureCount(assimp_enum); i++) {
        aiString path;
        mat->GetTexture(assimp_enum, i, &path);

        std::string full_path = (working_dir + "/" + path.C_Str());
        index = VTHasN(textures, GetNotDir(full_path).c_str());

        if (index == -1) {
            Texture texture = LoadImageTT(full_path.c_str());

            index = textures->size();
            textures->push_back(texture);
        }
    }

    return index;
}

static Mesh ProcessMesh(aiMesh *mesh, glm::mat4 mesh_transform, const aiScene *scene, Model* model) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

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

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            vertex.texture_uv.x = mesh->mTextureCoords[0][i].x; 
            vertex.texture_uv.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.texture_uv.x = 0;
            vertex.texture_uv.y = 0;
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
    
    t3d::Mesh t3d_mesh;

    t3d_mesh.vertices = vertices;
    t3d_mesh.indices = indices;

    // TODO: aiTextureType_DISPLACEMENT
    t3d_mesh.textures[TEXTURE_TYPE_BASE] = LoadMaterialTextures(material, &model->textures, aiTextureType_DIFFUSE);
    t3d_mesh.textures[TEXTURE_TYPE_NORMAL] = LoadMaterialTextures(material, &model->textures, aiTextureType_NORMALS);
    t3d_mesh.textures[TEXTURE_TYPE_ROUGHNESS] = LoadMaterialTextures(material, &model->textures, aiTextureType_SHININESS);
    t3d_mesh.textures[TEXTURE_TYPE_METAL] = LoadMaterialTextures(material, &model->textures, aiTextureType_METALNESS);
    t3d_mesh.textures[TEXTURE_TYPE_SPECULAR] = LoadMaterialTextures(material, &model->textures, aiTextureType_SPECULAR);
    t3d_mesh.textures[TEXTURE_TYPE_HEIGHT] = LoadMaterialTextures(material, &model->textures, aiTextureType_HEIGHT);
    t3d_mesh.textures[TEXTURE_TYPE_OPACITY] = LoadMaterialTextures(material, &model->textures, aiTextureType_OPACITY);
    t3d_mesh.textures[TEXTURE_TYPE_AMBIENT] = LoadMaterialTextures(material, &model->textures, aiTextureType_AMBIENT);
    t3d_mesh.textures[TEXTURE_TYPE_SELF_ILLUMINATION] = LoadMaterialTextures(material, &model->textures, aiTextureType_EMISSIVE);
 
    return t3d_mesh;
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

void LoadModel(std::string const &path, t3d::Model* model) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs); // TODO: aiProcess_SortByPType -> lines + points

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ERROR << importer.GetErrorString() << std::endl;
        model->version = -1;
        return;
    }

    ProcessNode(scene->mRootNode, scene, model);

    model->version = T3D_VERSION;
}
