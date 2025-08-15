#pragma once

#include <external.hpp>

class Layout;

class VertexBuffer {

	private:

		GLuint vao = 0;
		GLuint vbo = 0;
		uint32_t stride = 0;
		uint32_t vertices = 0;
		GLenum type = 0;

	public:

		VertexBuffer() = default;

		void init(const Layout& layout, GLenum type);
		void close();

		/// Upload given data to the GPU
		void upload(uint8_t* data, size_t size);

		/// Draw buffer data using bound shader
		void draw();

};

template <typename V>
class BufferWriter {

	private:

		std::vector<V> vertices;
		VertexBuffer* buffer = nullptr;

	public:

		BufferWriter() = default;

		void init(VertexBuffer* buffer) {
			this->buffer = buffer;
		}

		/// Write vertex to buffer
		void push(V vertex) {
			vertices.push_back(vertex);
		}

		/// Upload written data to the underlying buffer
		void upload() {
			buffer->upload((uint8_t*) vertices.data(), vertices.size() * sizeof(V));
			vertices.clear();
		}

};
