#pragma once

class VertexBuffer {
public:
	VertexBuffer(const void *data, size_t size);
	~VertexBuffer();
	void bind() const;
	void unbind() const;

private:
	unsigned int renderer_id;
};
