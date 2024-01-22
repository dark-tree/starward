#pragma once

#include <external.hpp>
#include "wrapper.hpp"
#include "layout.hpp"

namespace gls {

	class Buffer {

		private:

			GLuint vao;
			GLuint vbo;
			uint32_t stride;
			uint32_t vertices;
			GLenum type;

		public:
		
			Buffer(const Layout& layout, GLenum type) {
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

			~Buffer() {
				glDeleteVertexArrays(1, &vao);
				glDeleteBuffers(1, &vbo);
			}

			void upload(uint8_t* data, size_t size) {
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, size, data, type);
				vertices = size / stride;

				//printf("stride=%d size=%d vertices=%d\n", stride, (int) size, vertices);
			}

			void draw() {
				glBindVertexArray(vao);
				glDrawArrays(GL_TRIANGLES, 0, vertices);
			}

	};

}
