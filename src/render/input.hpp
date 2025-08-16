#pragma once

#include <external.hpp>
#include "util/ring.hpp"

class InputState {

	private:

		float mouse_x;
		float mouse_y;
		float prev_mouse_x;
		float prev_mouse_y;

		// records the state of the keyboard keys
		std::unordered_map<Key, KeyState> keyboard;

	public:

		// history of the last 32 keys typed
		Ring<Key, 32> history;

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
			KeyState& ks = state().get(code);
			ks = (ks == KeyState::UP) ? KeyState::TYPED : KeyState::DOWN;

			if (ks == KeyState::TYPED) {
				state().history.push(code);
			}
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

		static bool isPressed(Key code) {
			return state().get(code) != KeyState::UP;
		}

		static void purge() {
			state().history.clear();
		}

		template<typename... Keys>
		static bool matchKeys(Keys... codes) {
			std::array<Key, sizeof...(codes)> keys = {codes...};

			if (state().history.size() < keys.size()) {
				return false;
			}

			for (int i = 0; i < keys.size(); i++) {
				if (keys[i] != state().history.get(keys.size() - i)) return false;
			}

			return true;
		}

};
