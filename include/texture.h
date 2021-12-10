#pragma once

#include <string>

class Texture {
public:
	void bind() const;
	void unbind() const;
	Texture(const std::string &image_src_path);
	~Texture();

private:
	void load_image(const std::string& image_src_path) const;

private:
	unsigned int renderer_id;
};