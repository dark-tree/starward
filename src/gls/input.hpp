#pragma once

#include <external.hpp>

namespace gls {

	struct KeyScope {
		enum KeyEnum : int32_t {
			LEFT   = 37,
			RIGHT  = 39,
			UP     = 38,
			DOWN   = 40,
			SPACE  = 32,
			ESCAPE = 27,
		};
	};

	struct KeyStateScope {
		enum KeyStateEnum : int32_t {
			UP    = 0b00,
			DOWN  = 0b01,
			TYPED = 0b11,
		};
	};

	// export enums
	using Key = KeyScope::KeyEnum;
	using KeyState = KeyStateScope::KeyStateEnum;

	class InputState {

		private:

			float mouse_x;
			float mouse_y;
			float prev_mouse_x;
			float prev_mouse_y;

			// records the state of the keyboard keys
			std::unordered_map<int32_t, KeyState> keyboard;

		public:

			/// moves the virtual mouse
			void move(float x, float y) {
				prev_mouse_x = mouse_x;
				prev_mouse_y = mouse_y;
				mouse_x = x;
				mouse_y = y;
			}

			/// returns the state of the given key
			KeyState& get(int32_t key) {

				// if key is not in the map the underlying integral type of the
				// KeyState enum will be initilized to 0, which corresponds to KeyState::UP
				return keyboard[key];
			}

			/// resets the state for the next frame
			void clear() {

				for (auto& [key, state] : keyboard) {

					// changes KeyState::TYPED to KeyState::DOWN
					((int32_t&) state) &= 0b01;
				}
			}

	};

	class Input {

		private:

			static InputState& state() {
				static InputState state;
				return state;
			}

		public:

			static void press(int32_t code) {
				KeyState& key = state().get(code);
				key = (key == KeyState::UP) ? KeyState::TYPED : KeyState::DOWN;
			}

			static void release(int32_t code) {
				state().get(code) = KeyState::UP;
			}

			static void move(float x, float y) {
				state().move(x, y);
			}

			static void clear() {
				state().clear();
			}

		public:

			static bool is_pressed(int32_t code) {
				return state().get(code) != KeyState::UP;
			}

			static bool is_typed(int32_t code) {
				return state().get(code) == KeyState::TYPED;
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
