#pragma once

#include <external.hpp>
#include "wrapper.hpp"

class Shader {

	private:

		GLuint program = 0;

	public:

		Shader() = default;

		void init(const std::string& base_path);
		void close();

		/// Get uniform location by name
		int uniform(const char* name);

		/// Get attribute location by name
		int attribute(const char* name);

		/// Bind shader
		void use();

	public:

		/// Compile shader from GLSL string
		static GLuint compileShaderSource(GLenum type, const char* source);

		/// Link shader program from two shader modules
		static GLuint linkShaderProgram(GLuint vertex, GLuint fragment);

};

