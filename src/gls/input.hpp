#pragma once

namespace gls {

	std::unordered_map<int, bool>& get_keyboard() {
		static std::unordered_map<int, bool> keyboard;
		return keyboard;
	}

	struct Input {

		static void press(int code) {
			get_keyboard()[code] = true;
		}

		static void release(int code) {
			get_keyboard()[code] = false;
		}

		static bool is_pressed(int code) {
			return get_keyboard()[code];
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

}
