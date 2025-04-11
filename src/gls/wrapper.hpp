#pragma once

#include "input.hpp"
#include <external.hpp>
#include <util.hpp>

extern std::function<void()> __main_loop_func;

inline void __main_loop() {
	__main_loop_func();
}

namespace gls {

	struct Rectangle {
		int width;
		int height;
	};

	inline void init() {
		platform_init();

		// configure STB image
		stbi_set_flip_vertically_on_load(true);

		// set event handlers
		platform_set_keydown_callback(Input::press);
		platform_set_keyup_callback(Input::release);
//		platform_set_mouse_callback(Input::move);
	}

	inline Rectangle get_canvas_size() {
		Rectangle rect;
		platform_get_canvas_element_size(&rect.width, &rect.height);
		return rect;
	}

	inline void main_loop(const std::function<void()>& loop) {
		__main_loop_func = loop;
		platform_set_main_loop(__main_loop, 0);
	}

}
