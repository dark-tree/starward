#pragma once

#include "const.hpp"

using PlatformKeyEventCallback = void(*)(int);
using PlatformMouseEventCallback = void(*)(float, float);
using PlatformLoopCallback = void(*)();

#if defined(__EMSCRIPTEN__)

	#include <emscripten.h>
	#include <emscripten/html5.h>
	#include <GLES3/gl3.h>

	#define EXPORTED_NATIVE EMSCRIPTEN_KEEPALIVE

	namespace __impl {

		inline EM_BOOL __platform_keydown_handler (int type, const EmscriptenKeyboardEvent* event, void* userdata) {
			reinterpret_cast<PlatformKeyEventCallback>(userdata)(event->keyCode);
			return true;
		}

		inline EM_BOOL __platform_keyup_handler (int type, const EmscriptenKeyboardEvent* event, void* userdata) {
			reinterpret_cast<PlatformKeyEventCallback>(userdata)(event->keyCode);
			return true;
		}

		inline EM_BOOL __platform_mousemove_handler (int type, const EmscriptenMouseEvent* event, void* userdata) {
			reinterpret_cast<PlatformMouseEventCallback>(userdata)(event->clientX, event->clientY);
			return true;
		}

	}

	[[noreturn]] inline void platform_exit(int code) {
		emscripten_force_exit(code);
	}

	inline void platform_set_keydown_callback(PlatformKeyEventCallback callback) {
		emscripten_set_keydown_callback(HTML_CANVAS, (void*) callback, false, __impl::__platform_keydown_handler);
	}

	inline void platform_set_keyup_callback(PlatformKeyEventCallback callback) {
		emscripten_set_keyup_callback(HTML_CANVAS, (void*) callback, false, __impl::__platform_keyup_handler);
	}

	inline void platform_get_canvas_element_size(int* width, int* height) {
		emscripten_get_canvas_element_size(HTML_CANVAS, width, height);
	}

	inline void platform_set_main_loop(PlatformLoopCallback callback, int fps) {
		emscripten_set_main_loop((em_callback_func) callback, fps, true);
	}

	inline void platform_init() {
		printf("Began 'Emscripten/WebGL' platform init...\n");

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
		int context = emscripten_webgl_create_context(HTML_CANVAS, &attrs);

		if (context == 0) {
			printf("Failed to create WebGL context!\n");
			platform_exit(-1);
		}

		// and bind it the the current "thread"
		emscripten_webgl_make_context_current(context);
	}

#elif defined(__linux__)

	// sudo apt-get install libopenal-dev

	#include <winx/winx.h>
	#include <glad/glad.h>

	#define EXPORTED_NATIVE

	namespace __impl {

		extern PlatformKeyEventCallback __keydown_callback;
		extern PlatformKeyEventCallback __keyup_callback;

		inline void __platform_close_handler() {
			exit(-1);
		}

		inline void __platform_keyboard_handler(int state, int keycode) {
			if (state == WINX_PRESSED) {
				__impl::__keydown_callback(keycode);
			} else {
				__impl::__keyup_callback(keycode);
			}
		}

	}

	[[noreturn]] inline void platform_exit(int code) {
		exit(code);
	}

	inline void platform_set_keydown_callback(PlatformKeyEventCallback callback) {
		__impl::__keydown_callback = callback;
	}

	inline void platform_set_keyup_callback(PlatformKeyEventCallback callback) {
		__impl::__keyup_callback = callback;
	}

	inline void platform_get_canvas_element_size(int* width, int* height) {
		*width = 400;
		*height = 300;
	}

	inline void platform_set_main_loop(PlatformLoopCallback callback, int fps) {
		while (true) {
			callback();

			winxSwapBuffers();
			winxPollEvents();
		}
	}

	inline void platform_init() {
		printf("Began 'Winx/OpenGL' platform init...\n");

		winxHint(WINX_HINT_VSYNC, WINX_VSYNC_ENABLED);

		if (!winxOpen(400, 300, "WINX OpenGL 3.0")) {
			printf("Failed to create WINX context! %s\n", winxGetError());
			platform_exit(1);
		}

		// use GLAD to load OpenGL functions
		gladLoadGL();

		winxSetCloseEventHandle(__impl::__platform_close_handler);
		winxSetKeybordEventHandle(__impl::__platform_keyboard_handler);
	}

#endif
