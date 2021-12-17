#pragma once

#include "mesh.h"
#include "shader.h"

#include <vector>
#include <string>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/material.h>

class Model {
public:
    Model(const std::string &path);
    void draw(Shader &shader);

private:
    std::vector<Mesh> m_meshes;
    std::string m_directory;

private:
    void load_model(const std::string &path);
    void process_node(aiNode *node, const aiScene *scene);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> load_material_textures(aiMaterial *material,
            aiTextureType type,
            TextureType texture_type);
};
