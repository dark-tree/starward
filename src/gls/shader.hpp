#pragma once

#include <external.hpp>
#include "wrapper.hpp"

namespace gls {

	class Shader {

		private:

			GLuint program;

		public:

			Shader(const char* vertex_source, const char* fragment_source) {
				GLuint vert = gls::compile_shader(GL_VERTEX_SHADER, vertex_source);
				GLuint frag = gls::compile_shader(GL_FRAGMENT_SHADER, fragment_source);
				program = gls::link_shaders(vert, frag);

				glDeleteShader(vert);
				glDeleteShader(frag);
			}

			~Shader() {
				glDeleteProgram(program);
			}

			int uniform(const char* name) {
				return glGetUniformLocation(program, name);
			}

			int attribute(const char* name) {
				return glGetAttribLocation(program, name);
			}

			void use() {
				glUseProgram(program);
			}

	};

}
