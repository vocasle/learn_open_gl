#pragma once

#include <glm/glm.hpp>

#include <vector>

#include "vertex_array.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "shader.h"

struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
};

enum class TextureType {
        DIFFUSE, SPECULAR
};

struct Texture {
        unsigned int id;
        TextureType type;
        std::string path;
};

class Mesh {
public:
        Mesh(const std::vector<Vertex> &vertices,
                        const std::vector<unsigned int> &indices,
                        const std::vector<Texture> &textures);
        void draw(Shader &shader);

private:
        VertexArray m_va;
        IndexBuffer m_ib;
        VertexBuffer m_vb;
        std::vector<Vertex>             m_vertices;
        std::vector<unsigned int>       m_indices;
        std::vector<Texture>            m_textures;

private:

        void setup_mesh();
        
};
