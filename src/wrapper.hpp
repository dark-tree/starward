#pragma once

std::function<void()> __main_loop_func;

inline void __main_loop() { 
	__main_loop_func(); 
}

namespace gls {

	inline void webgl_init() {
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
		int context = emscripten_webgl_create_context("canvas", &attrs);
		if (context == 0) {
		    printf("Failed to create WebGL context!\n");
		    exit(-1);
		}    
	
		// and bind it the the current "thread"
		emscripten_webgl_make_context_current(context);
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

			printf("Shader compilation failed with error: %s\n", error.data());
			exit(-1);
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

			printf("Failed to link shader program!\nLinking failed with error: %s\n", error.data());
			exit(-1);
		}

		return program;

	}

}

