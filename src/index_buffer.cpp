#include <glad/glad.h>

#include "index_buffer.h"
#include "utility.h"


IndexBuffer::IndexBuffer(const void *data, size_t size):renderer_id(0)
{
        GL_CALL(glGenBuffers(1, &renderer_id));
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id));
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                size * sizeof(unsigned int),
                                data,
                                GL_STATIC_DRAW));

}

void IndexBuffer::bind() const
{
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id));
}

void IndexBuffer::unbind() const
{
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

IndexBuffer::~IndexBuffer()
{
        GL_CALL(glDeleteBuffers(1, &renderer_id));
}
