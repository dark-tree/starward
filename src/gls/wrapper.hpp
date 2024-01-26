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

	inline void init(const char* canvas) {
		EmscriptenWebGLContextAttributes attrs;
		attrs.alpha = false;
		attrs.depth = true;
		attrs.stencil = true;
		attrs.antialias = true;
		attrs.premultipliedAlpha = false;
		attrs.preserveDrawingBuffer = false;
		attrs.powerPreference = false;
		attrs.failIfMajorPerformanceCaveat = true;
		attrs.majorVersion = 2;
		attrs.minorVersion = 0;
		attrs.enableExtensionsByDefault = false;

		// create the context
		int context = emscripten_webgl_create_context(canvas, &attrs);
		if (context == 0) {
			fault("Failed to create WebGL context!\n");
		}

		// and bind it the the current "thread"
		emscripten_webgl_make_context_current(context);

		// configure STB image
		stbi_set_flip_vertically_on_load(true);

		// set event handlers
		emscripten_set_keydown_callback(canvas, nullptr, false, gls::__keydown_handler);
		emscripten_set_keyup_callback(canvas, nullptr, false, gls::__keyup_handler);
	}

	inline Rectangle get_canvas_size(const char* canvas) {
		Rectangle rect;
		emscripten_get_canvas_element_size(canvas, &rect.width, &rect.height);
		return rect;
	}

	inline void main_loop(const std::function<void()>& loop) {
		__main_loop_func = loop;
		emscripten_set_main_loop(__main_loop, 0, true);
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
