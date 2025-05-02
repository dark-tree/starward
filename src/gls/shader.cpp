
#include "shader.hpp"

/*
 * Shader
 */

Shader::Shader(const std::string& base_path) {
	std::string vertex_source = readFile(base_path + ".vert");
	std::string fragment_source = readFile(base_path + ".frag");

	GLuint vert = compileShaderSource(GL_VERTEX_SHADER, vertex_source.c_str());
	GLuint frag = compileShaderSource(GL_FRAGMENT_SHADER, fragment_source.c_str());
	this->program = linkShaderProgram(vert, frag);

	glDeleteShader(vert);
	glDeleteShader(frag);
}

Shader::~Shader() {
	glDeleteProgram(program);
}

int Shader::uniform(const char* name) {
	return glGetUniformLocation(program, name);
}

int Shader::attribute(const char* name) {
	return glGetAttribLocation(program, name);
}

void Shader::use() {
	glUseProgram(program);
}

GLuint Shader::compileShaderSource(GLenum type, const char* source) {

	// create shader
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	// error checking
	GLint compiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (compiled == GL_FALSE) {
		GLint length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

		std::vector<char> error (length);
		glGetShaderInfoLog(shader, length, &length, error.data());

		fault("Shader compilation failed with error: %s\n", error.data());
	}

	return shader;
}

GLuint Shader::linkShaderProgram(GLuint vertex, GLuint fragment) {

	// create shader program
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	// error checking
	GLint linked = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if(linked == GL_FALSE) {
		GLint length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

		std::vector<char> error (length);
		glGetProgramInfoLog(program, length, &length, error.data());

		fault("Shader linking failed with error: %s\n", error.data());
	}

	return program;
}