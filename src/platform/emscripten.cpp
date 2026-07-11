
/*
 * EMSCRIPTEN implementation of the platform abstraction interface
 */

#include "platform.hpp"

#if defined(__EMSCRIPTEN__)

	/// translates the javascript's key code into key enum
	static Key platform_translate_key(const char* key) {

		if (strcmp(key, "ArrowLeft") == 0) return Key::LEFT;
		if (strcmp(key, "ArrowRight") == 0) return Key::RIGHT;
		if (strcmp(key, "ArrowUp") == 0) return Key::UP;
		if (strcmp(key, "ArrowDown") == 0) return Key::DOWN;
		if (strcmp(key, "Space") == 0) return Key::SPACE;
		if (strcmp(key, "Escape") == 0) return Key::ESCAPE;
		if (strcmp(key, "Tab") == 0) return Key::TAB;
		if (strcmp(key, "Enter") == 0) return Key::ENTER;
		if (strcmp(key, "KeyW") == 0) return Key::W;
		if (strcmp(key, "KeyA") == 0) return Key::A;
		if (strcmp(key, "KeyD") == 0) return Key::D;
		if (strcmp(key, "KeyB") == 0) return Key::B;

		return Key::UNDEF;
	}

	static EM_BOOL platform_keydown_handler(int type, const EmscriptenKeyboardEvent* event, void* userdata) {
		reinterpret_cast<PlatformKeyEventCallback>(userdata)(platform_translate_key(event->code));
		return true;
	}

	static EM_BOOL platform_keyup_handler(int type, const EmscriptenKeyboardEvent* event, void* userdata) {
		reinterpret_cast<PlatformKeyEventCallback>(userdata)(platform_translate_key(event->code));
		return true;
	}

	static EM_BOOL platform_mouse_handler(int type, const EmscriptenMouseEvent* event, void* userdata) {
		reinterpret_cast<PlatformMouseEventCallback>(userdata)(event->clientX, event->clientY, event->buttons != 0);
		return true;
	}

	static EM_BOOL platform_touch_handler(int type, const EmscriptenTouchEvent* event, void* userdata) {

		bool pressed = (type == EMSCRIPTEN_EVENT_TOUCHSTART) || (type == EMSCRIPTEN_EVENT_TOUCHMOVE);

		int i = event->numTouches;

		if (i == 0) {
			reinterpret_cast<PlatformMouseEventCallback>(userdata)(0, 0, false);
			return true;
		}

		float x = event->touches[0].clientX;
		float y = event->touches[0].clientY;

		reinterpret_cast<PlatformMouseEventCallback>(userdata)(x, y, pressed);
		return true;
	}

	/*
	 * Implementation
	 */

	std::string platform::get_startup_string() {
		std::string code = "location.search.split('i=').splice(1).join('').split('&')[0]";
		std::string data = emscripten_run_script_string(code.data());

		if (data == "null" || data == "undefined") {
			return "";
		}

		return data;
	}

	[[noreturn]] void platform::exit(int code) {
		emscripten_force_exit(code);
	}

	void platform::set_keydown_callback(PlatformKeyEventCallback callback) {
		emscripten_set_keydown_callback(HTML_CANVAS, (void*) callback, false, platform_keydown_handler);
	}

	void platform::set_keyup_callback(PlatformKeyEventCallback callback) {
		emscripten_set_keyup_callback(HTML_CANVAS, (void*) callback, false, platform_keyup_handler);
	}

	void platform::set_touch_callback(PlatformMouseEventCallback callback) {
		emscripten_set_mousemove_callback(HTML_CANVAS, (void*) callback, false, platform_mouse_handler);
		emscripten_set_mousedown_callback(HTML_CANVAS, (void*) callback, false, platform_mouse_handler);
		emscripten_set_mouseup_callback(HTML_CANVAS, (void*) callback, false, platform_mouse_handler);
		emscripten_set_mouseup_callback(HTML_CANVAS, (void*) callback, false, platform_mouse_handler);

		emscripten_set_touchstart_callback(HTML_CANVAS, (void*) callback, false, platform_touch_handler);
		emscripten_set_touchend_callback(HTML_CANVAS, (void*) callback, false, platform_touch_handler);
		emscripten_set_touchmove_callback(HTML_CANVAS, (void*) callback, false, platform_touch_handler);
		emscripten_set_touchcancel_callback(HTML_CANVAS, (void*) callback, false, platform_touch_handler);
	}

	void platform::get_render_target_size(int* width, int* height) {
		emscripten_get_canvas_element_size(HTML_CANVAS, width, height);
	}

	void platform::set_main_loop(PlatformLoopCallback callback, int fps) {
		emscripten_set_main_loop((em_callback_func) callback, fps, true);

		// this should not be reached
		platform::exit(1);
	}

	void platform::init() {
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
			platform::exit(-1);
		}

		// and bind it to the current "thread"
		emscripten_webgl_make_context_current(context);
	}

	void platform::write_string(std::string path, std::string data) {
		std::string code = "localStorage.setItem('" + path + "', '" + data + "');";
		emscripten_run_script(code.data());
		printf("Written '%s' into '/%s'\n", data.c_str(), path.c_str());
	}

	std::string platform::read_string(std::string path) {
		std::string code = "localStorage.getItem('" + path + "');";
		std::string data = emscripten_run_script_string(code.data());

		if (data == "null" || data == "undefined") {
			return "";
		}

		printf("Read '%s' from '/%s'\n", data.c_str(), path.c_str());

		return data;
	}

	void platform::ready() {
		emscripten_run_script("onGameReady()");
	}


#endif