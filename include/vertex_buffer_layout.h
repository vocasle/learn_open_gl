#pragma once
#include <vector>
#include <glad/glad.h>

struct VertexBufferElement
{
	unsigned int count;
	unsigned int type;
	unsigned char normalized;
	static unsigned int get_type_size(int i);
};

class VertexBufferLayout
{
public:
	VertexBufferLayout();
	~VertexBufferLayout();

	template <typename T>
	void add_element(unsigned int count);
	const std::vector<VertexBufferElement>& get_elements() const;
	unsigned int get_stride() const;

private:
	std::vector<VertexBufferElement> elements;
	unsigned int stride;

};

template <typename T>
void VertexBufferLayout::add_element(unsigned int count)
{
	static_assert(false);
}

template <>
inline void VertexBufferLayout::add_element<float>(unsigned int count)
{
	elements.emplace_back(count, static_cast<unsigned int>(GL_FLOAT), static_cast<unsigned char>(GL_FALSE));
	stride += VertexBufferElement::get_type_size(GL_FLOAT) * count;
}