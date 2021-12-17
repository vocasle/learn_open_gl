#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include <iostream>


Model::Model(const std::string &path)
{
        load_model(path);
}

void Model::draw(Shader &shader)
{
        for (unsigned int i = 0; i < m_meshes.size(); ++i)
                m_meshes[i].draw(shader);
}



void Model::load_model(const std::string &path)
{
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path.c_str(),
                        aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cerr << "ERROR::ASSIMP::"
                        << importer.GetErrorString() << std::endl;
                return;
        }

        m_directory = path.substr(0, path.find_last_of('/'));
        process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode *node, const aiScene *scene)
{
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                m_meshes.push_back(process_mesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
                process_node(node->mChildren[i], scene);
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                Vertex vertex;
                vertex.Position.x = mesh->mVertices[i].x;
                vertex.Position.y = mesh->mVertices[i].y;
                vertex.Position.z = mesh->mVertices[i].z;
                vertex.Normal.x = mesh->mNormals[i].x;
                vertex.Normal.y = mesh->mNormals[i].y;
                vertex.Normal.z = mesh->mNormals[i].z;
                if (mesh->mTextureCoords[0]) {
                        vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
                        vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
                }
                else 
                        vertex.TexCoords = glm::vec2(0.0f);
                vertices.push_back(vertex);
        }
        
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; ++j)
                        indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0) {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                std::vector<Texture> diffuse_maps = load_material_textures(
                                material,
                                aiTextureType_DIFFUSE,
                                TextureType::DIFFUSE);
                textures.insert(textures.end(), 
                                std::begin(diffuse_maps), 
                                std::end(diffuse_maps));
                std::vector<Texture> specular_maps = load_material_textures(
                                material,
                                aiTextureType_SPECULAR,
                                TextureType::SPECULAR);
                textures.insert(textures.end(),
                                std::begin(specular_maps),
                                std::end(specular_maps));
        }

        return Mesh(vertices, indices, textures);
}

unsigned int texture_from_file(const char *path, const std::string &directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

std::vector<Texture> Model::load_material_textures(aiMaterial *material,
    aiTextureType type,
    TextureType texture_type)
{
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < material->GetTextureCount(type); ++i) {
                aiString str;
                material->GetTexture(type, i, &str);
                Texture texture;
                texture.id = texture_from_file(str.C_Str(), m_directory, false); 
                texture.type = texture_type;
                texture.path = str.C_Str();
                textures.push_back(texture);
        }
        return textures;
}











