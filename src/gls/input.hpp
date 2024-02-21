#pragma once

#include <external.hpp>

namespace gls {

	class InputState {

		private:

			float mouse_x;
			float mouse_y;
			float prev_mouse_x;
			float prev_mouse_y;

			// records the state of the keyboard keys
			std::unordered_map<Key, KeyState> keyboard;

		public:

			/// moves the virtual mouse
			void move(float x, float y) {
				prev_mouse_x = mouse_x;
				prev_mouse_y = mouse_y;
				mouse_x = x;
				mouse_y = y;
			}

			/// returns the state of the given key
			KeyState& get(Key key) {

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

			static void press(Key code) {
				KeyState& key = state().get(code);
				key = (key == KeyState::UP) ? KeyState::TYPED : KeyState::DOWN;
			}

			static void release(Key code) {
				state().get(code) = KeyState::UP;
			}

			static void move(float x, float y) {
				state().move(x, y);
			}

			static void clear() {
				state().clear();
			}

		public:

			static bool is_pressed(Key code) {
				return state().get(code) != KeyState::UP;
			}

			static bool is_typed(Key code) {
				return state().get(code) == KeyState::TYPED;
			}

	};

}
