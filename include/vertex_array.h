#pragma once

#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

class VertexArray {
public:
	VertexArray();
	~VertexArray();
	void bind() const;
	void unbind() const;
	void add_buffer(const VertexBuffer& vb, const VertexBufferLayout& vbl);

private:
	unsigned int renderer_id;

};