#include <glad/glad.h>

#include "vertex_buffer.h"
#include "utility.h"

VertexBuffer::~VertexBuffer()
{
	GL_CALL(glDeleteBuffers(1, &renderer_id));
}

VertexBuffer::VertexBuffer(const void* data, size_t size): renderer_id(0)
{
	GL_CALL(glGenBuffers(1, &renderer_id));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer_id));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void VertexBuffer::bind() const
{
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer_id));
}

void VertexBuffer::unbind() const
{
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}