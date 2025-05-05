#pragma once

#include <external.hpp>

class Layout;

class VertexBuffer {

	private:

		GLuint vao;
		GLuint vbo;
		uint32_t stride;
		uint32_t vertices;
		GLenum type;

	public:

		VertexBuffer(const Layout& layout, GLenum type);
		~VertexBuffer();

		VertexBuffer(const VertexBuffer& buffer) = delete;
		VertexBuffer(VertexBuffer&& buffer) = default;

		/// Upload given data to the GPU
		void upload(uint8_t* data, size_t size);

		/// Draw buffer data using bound shader
		void draw();

};

template <typename V>
class BufferWriter {

	private:

		std::vector<V> vertices;
		VertexBuffer& buffer;

	public:

		BufferWriter(VertexBuffer& buffer)
		: buffer(buffer) {}

		/// Write vertex to buffer
		void push(V vertex) {
			vertices.push_back(vertex);
		}

		/// Upload written data to the underlying buffer
		void upload() {
			buffer.upload((uint8_t*) vertices.data(), vertices.size() * sizeof(V));
			vertices.clear();
		}

};
