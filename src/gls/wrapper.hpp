#pragma once

#include "input.hpp"
#include <external.hpp>
#include <util.hpp>

std::function<void()> __main_loop_func;

inline void __main_loop() {
	__main_loop_func();
}

namespace gls {

	struct Rectangle {
		int width;
		int height;
	};

	inline void init() {
		platform_init();

		// configure STB image
		stbi_set_flip_vertically_on_load(true);

		// set event handlers
		platform_set_keydown_callback(Input::press);
		platform_set_keyup_callback(Input::release);
//		platform_set_mouse_callback(Input::move);
	}

	inline Rectangle get_canvas_size() {
		Rectangle rect;
		platform_get_canvas_element_size(&rect.width, &rect.height);
		return rect;
	}

	inline void main_loop(const std::function<void()>& loop) {
		__main_loop_func = loop;
		platform_set_main_loop(__main_loop, 0);
	}

	inline GLuint compile_shader(GLenum type, const char* source) {

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

	inline GLuint link_shaders(GLuint vertex, GLuint fragment) {

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

}
