#include "mesh.h"

#include <glad/glad.h>
#include "utility.h"

#include <string>


Mesh::Mesh(const std::vector<Vertex> &vertices,
                        const std::vector<unsigned int> &indices,
                        const std::vector<Texture> &textures):
        m_vertices(vertices),
        m_indices(indices),
        m_textures(textures),
        m_vb(vertices.data(), vertices.size() * sizeof(Vertex)),
        m_ib(indices.data(), indices.size() * sizeof(unsigned int)),
        m_va()
{
        setup_mesh();
}

void Mesh::setup_mesh()
{
        VertexBufferLayout vbl;
        vbl.add_element<Vertex>(3);
        vbl.add_element<Vertex>(3);
        vbl.add_element<Vertex>(2);
        m_va.add_buffer(m_vb, vbl);
}

std::string text_type_to_str(TextureType type)
{
        if (type == TextureType::DIFFUSE)
                return "diffuse";
        else
                return "specular";
}

void Mesh::draw(Shader &shader)
{
        unsigned int diffuse_num = 1;
        unsigned int specular_num = 1;

        for (unsigned int i = 0; i < m_textures.size(); ++i) {
               GL_CALL(glActiveTexture(GL_TEXTURE0 + i)); 

               shader.set_float(format("material.{}{}",
                                       text_type_to_str(m_textures[i].type),
                                       m_textures[i].type == TextureType::DIFFUSE 
                                       ? diffuse_num++
                                       : specular_num++
                                      ),
                               i);
               GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textures[i].id));
        }

        GL_CALL(glActiveTexture(GL_TEXTURE0));

        m_va.bind();
        GL_CALL(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr));
}











