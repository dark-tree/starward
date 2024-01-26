#pragma once

#include <external.hpp>

namespace gls {

	enum struct Key : int32_t {

		LEFT   = 37,
		RIGHT  = 39,
		UP     = 38,
		DOWN   = 40,
		SPACE  = 32,
		ESCAPE = 27,

	};

	class Input {

			static float mx, my, px, py;

			static std::unordered_map<int, bool>& get_keyboard() {
				static std::unordered_map<int, bool> keyboard;
				return keyboard;
			}

		public:

			static void press(int32_t code) {
				get_keyboard()[code] = true;
			}

			static void release(int32_t code) {
				get_keyboard()[code] = false;
			}

			static void move(float x, float y) {
				px = mx;
				py = my;
				mx = x;
				my = y;
			}

		public:

			static bool is_pressed(int32_t code) {
				return get_keyboard()[code];
			}

			static bool is_pressed(Key key) {
				return is_pressed((int32_t) key);
			}

	};

	EM_BOOL __keydown_handler (int type, const EmscriptenKeyboardEvent* event, void* userdata) {
		Input::press(event->keyCode);
		return true;
	}

	EM_BOOL __keyup_handler (int type, const EmscriptenKeyboardEvent* event, void* userdata) {
		Input::release(event->keyCode);
		return true;
	}

	EM_BOOL __mousemove_handler (int type, const EmscriptenMouseEvent* event, void* userdata) {
		Input::move(event->clientX, event->clientY);
		return true;
	}

}
