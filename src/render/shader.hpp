#pragma once

#include <external.hpp>
#include "wrapper.hpp"

class Shader {

	private:

		GLuint program;

	public:

		Shader(const std::string& base_path);
		~Shader();

		Shader(const Shader& buffer) = delete;
		Shader(Shader&& buffer) = default;

		/// Get uniform location by name
		int uniform(const char* name);

		/// Get attribute location by name
		int attribute(const char* name);

		/// Bind shader
		void use();

	public:

		/// Compile shader from GLSL string
		static GLuint compileShaderSource(GLenum type, const char* source);

		/// link shader program from two shader modules
		static GLuint linkShaderProgram(GLuint vertex, GLuint fragment);

};

