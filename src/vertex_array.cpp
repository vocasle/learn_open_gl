#include "vertex_array.h"
#include "utility.h"

VertexArray::VertexArray(): renderer_id(0)
{
	GL_CALL(glGenVertexArrays(1, &renderer_id));
	GL_CALL(glBindVertexArray(renderer_id));
}

VertexArray::~VertexArray()
{
	GL_CALL(glDeleteVertexArrays(1, &renderer_id));
}

void VertexArray::bind() const
{
	GL_CALL(glBindVertexArray(renderer_id));
}

void VertexArray::unbind() const
{
	GL_CALL(glBindVertexArray(0));
}

void VertexArray::add_buffer(const VertexBuffer& vb, const VertexBufferLayout& vbl)
{
	vb.bind();
	unsigned int offset = 0;
	for (size_t i = 0; const auto &element : vbl.get_elements())
	{
		GL_CALL(glVertexAttribPointer(i, element.count, element.type, element.normalized, vbl.get_stride(), reinterpret_cast<void*>(offset)));
		GL_CALL(glEnableVertexAttribArray(i));
		offset += element.count * VertexBufferElement::get_type_size(element.type);
		++i;
	}
}
