
#include "buffer.hpp"
#include "layout.hpp"

/*
 * VertexBuffer
 */

VertexBuffer::VertexBuffer(const Layout& layout, GLenum type) {
	// create and bind VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create and bind VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// configure VAO
	this->stride = layout.apply();
	this->type = type;
}

VertexBuffer::~VertexBuffer() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void VertexBuffer::upload(uint8_t* data, size_t size) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, type);
	vertices = size / stride;
}

void VertexBuffer::draw() {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
}