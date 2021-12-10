#include "texture.h"

#include <glad/glad.h>

#include "utility.h"

#include <string>
#include <iostream>

#include <stb_image.h>

Texture::Texture(const std::string &image_src_path) : renderer_id(0)
{
	GL_CALL(glGenTextures(1, &renderer_id));
    load_image(image_src_path);
}

void Texture::load_image(const std::string& image_src_path) const
{
    int width = 0;
    int height = 0;
    int text_channels = 0;
    unsigned char *data = stbi_load(
        image_src_path.c_str(),
        &width,
        &height,
        &text_channels,
        0
    );

    if (data) {
        GLenum format;
        if (text_channels == 1)
            format = GL_RED;
        else if (text_channels == 3)
            format = GL_RGB;
        else if (text_channels == 4)
            format = GL_RGBA;

        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    }
    else {
        std::cerr << "Failed to load texture from file "
            << image_src_path << std::endl;
    }
    stbi_image_free(data);
}

void Texture::bind() const
{
	GL_CALL(glBindTexture(GL_TEXTURE_2D, renderer_id));
}

void Texture::unbind() const
{
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::~Texture()
{
    unbind();
    GL_CALL(glDeleteTextures(1, &renderer_id));
}