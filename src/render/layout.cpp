#include "layout.hpp"

/*
 * Attribute
 */

Layout::Attribute::Attribute(int index, int count, GLenum type, bool normalize, int size, int offset)
: index(index), count(count), type(type), normalize(normalize), size(size), offset(offset) {}

/*
 * Layout
 */

int Layout::glTypeSize(GLenum type) {
	if (type == GL_BYTE) return sizeof(uint8_t);
	if (type == GL_UNSIGNED_BYTE) return sizeof(uint8_t);
	if (type == GL_SHORT) return sizeof(uint16_t);
	if (type == GL_UNSIGNED_SHORT) return sizeof(uint16_t);
	if (type == GL_INT) return sizeof(uint32_t);
	if (type == GL_UNSIGNED_INT) return sizeof(uint32_t);
	if (type == GL_FLOAT) return sizeof(float);

	fault("Invalid OpenGL data type!\n");
}

uint32_t Layout::size() const {
	uint32_t size = 0;

	for (const Attribute& attr : attributes) {
		size += attr.size;
	}

	return size;
}

uint32_t Layout::apply() const {
	const uint32_t stride = size();

	for (const Attribute& attr : attributes) {
		glVertexAttribPointer(attr.index, attr.count, attr.type, attr.normalize, stride, reinterpret_cast<void*>(attr.offset));
		glEnableVertexAttribArray(attr.index);
	}

	return stride;
}

void Layout::attribute(int index, int count, GLenum type, bool normalize) {
	const int offset = size();
	attributes.emplace_back(index, count, type, normalize, glTypeSize(type) * count, offset);
}