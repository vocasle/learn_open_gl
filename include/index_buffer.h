#pragma once

class IndexBuffer {
public:
	IndexBuffer(const void *data, size_t size);
	~IndexBuffer();
	void bind() const;
	void unbind() const;

private:
	unsigned int renderer_id;
};
