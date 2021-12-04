#include "vertex_buffer_layout.h"

#include "utility.h"

unsigned int VertexBufferElement::get_type_size(int i)
{
	switch(i)
	{
	case GL_FLOAT:
		return sizeof(float);
	default:
		return 0;
	}
}

VertexBufferLayout::VertexBufferLayout(): elements(), stride(0)
{
}

VertexBufferLayout::~VertexBufferLayout()
{
}

const std::vector<VertexBufferElement>& VertexBufferLayout::get_elements() const
{
	return elements;
}

unsigned int VertexBufferLayout::get_stride() const
{
	return stride;
}
