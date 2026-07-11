#pragma once

#if defined(__EMSCRIPTEN__)
	#include <emscripten.h>
	#include <emscripten/html5.h>
	#include <GLES3/gl3.h>

	#define EXPORTED_NATIVE EMSCRIPTEN_KEEPALIVE
#endif

#if defined(__linux__)
	#include <fstream>
	#include <winx.h>
	#include <glad/glad.h>
	#include <sys/stat.h>

	#define EXPORTED_NATIVE
#endif

#include <string>

#include "const.hpp"

enum struct Key : unsigned int {
	UNDEF = 0,

	LEFT   = 37,
	RIGHT  = 39,
	UP     = 38,
	DOWN   = 40,
	SPACE  = 32,
	ESCAPE = 27,
	TAB    = 9,
	ENTER  = 13,
	W      = 87,
	A      = 65,
	D      = 68,
	B      = 66,
};

// event callback types
using PlatformKeyEventCallback = void(*)(Key);
using PlatformMouseEventCallback = void(*)(float, float, bool);
using PlatformLoopCallback = void(*)();

namespace platform {

	/// save a key-value pair to persistent storage (filesystem, local storage), read using read_string()
	void write_string(std::string path, std::string data);

	/// read a key-value pair from persistent storage (filesystem, local storage), save using write_string()
	std::string read_string(std::string path);

	/// Get startup string
	std::string get_startup_string();

	/// Get startup parameter
	int get_startup_param();

	/// Terminate program execution
	[[noreturn]] void exit(int code);

	/// Set a callback that will be invoked when a keyboard key is pressed
	void set_keydown_callback(PlatformKeyEventCallback callback);

	/// Set a callback that will be invoked when a keyboard key is released
	void set_keyup_callback(PlatformKeyEventCallback callback);

	/// Set a callback that will be invoked when a point is touched
	void set_touch_callback(PlatformMouseEventCallback callback);

	/// Get the dimensions (in pixels) of our canvas
	void get_render_target_size(int* width, int* height);

	/// Set a function to be invoked for each frame drawn
	[[noreturn]] void set_main_loop(PlatformLoopCallback callback, int fps);

	/// Initialize the graphics system, prepare canvas
	void init();

	/// Call when game is read yto play to hide the loading screen overlay
	void ready();

}